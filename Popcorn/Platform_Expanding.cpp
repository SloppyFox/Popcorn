#include "Platform_Expanding.h"

// AsPlatform_Expanding
const double AsPlatform_Expanding::Max_Expanding_Width = 40.0;
const double AsPlatform_Expanding::Min_Expanding_Width = (double)AsConfig::Platform_Normal_Width;
const double AsPlatform_Expanding::Expanding_Width_Step = 1.0;
//------------------------------------------------------------------------------------------------------------
AsPlatform_Expanding::~AsPlatform_Expanding()
{
	delete Truss_Color;
}
//------------------------------------------------------------------------------------------------------------
AsPlatform_Expanding::AsPlatform_Expanding(AsPlatform_State &platform_state)
	: Platform_State(&platform_state), Truss_Color(0), Inner_Color(0), Highlight_Color(0), Circle_Color(0),
	Expanding_Finalize_Timer_Tick(0), Curr_Expanding_Width(0)
{
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Expanding::Init(AColor &inner_color, AColor &circle_color, AColor &highlight_color)
{
	Inner_Color = &inner_color;
	Circle_Color = &circle_color;
	Highlight_Color = &highlight_color;

	Truss_Color = new AColor(inner_color, AsConfig::Global_Scale);
}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform_Expanding::Act(double &x_pos, EPlatform_State &next_state)
{
	next_state = EPlatform_State::Unknown;

	switch (Platform_State->Expanding)
	{
	case EPlatform_Transformation::Init:
		if (Curr_Expanding_Width < Max_Expanding_Width)
		{
			Curr_Expanding_Width += Expanding_Width_Step;
			x_pos -= Expanding_Width_Step / 2.0;
		}
		else
		{
			Platform_State->Expanding = EPlatform_Transformation::Active;
			Curr_Expanding_Width = Max_Expanding_Width;
			Expanding_Finalize_Timer_Tick = AsConfig::Current_Timer_Tick + AsConfig::Platform_Expanded_Timeout;
		}

		return true;
		break;


	case EPlatform_Transformation::Active:
		if (AsConfig::Current_Timer_Tick == Expanding_Finalize_Timer_Tick)
			Platform_State->Expanding = EPlatform_Transformation::Finalize;
		break;


	case EPlatform_Transformation::Finalize:
		if (Curr_Expanding_Width > Min_Expanding_Width)
		{
			Curr_Expanding_Width -= Expanding_Width_Step;
			x_pos += Expanding_Width_Step / 2.0;

		}
		else
		{
			Platform_State->Expanding = EPlatform_Transformation::Unknown;
			next_state = Platform_State->Set_State(EPlatform_Substate_Regular::Normal);
		}
		return true;

	default:
		AsTools::Throw();
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Expanding::Draw_State(HDC hdc, double x_pos)
{
	const int scale = AsConfig::Global_Scale;
	const double d_scale = AsConfig::D_Global_Scale;
	int expanding_center_x, platform_left, platform_right;
	RECT inner_rect{};

	expanding_center_x = (int)( (x_pos + Curr_Expanding_Width / 2.0) * d_scale);
	platform_left = (int)(expanding_center_x - (Curr_Expanding_Width / 2.0) * d_scale);
	platform_right = (int)(expanding_center_x + (Curr_Expanding_Width / 2.0) * d_scale);

	inner_rect.left = expanding_center_x - Expanding_Inner_Width / 2 * scale;
	inner_rect.top = (AsConfig::Platform_Y_Pos + 1) * scale;
	inner_rect.right = expanding_center_x + Expanding_Inner_Width / 2 * scale;
	inner_rect.bottom = (AsConfig::Platform_Y_Pos - 1 + AsConfig::Platform_Circle_Diameter) * AsConfig::Global_Scale;

	// 1. Шарики
	Draw_Expanding_Part(hdc, platform_left, inner_rect, true);
	Draw_Expanding_Part(hdc, platform_left, inner_rect, false);

	// 2. Внутрення часть
	AsTools::Rect(hdc, inner_rect, *Inner_Color);

	// 3. Блик
	Draw_Circle_Highlight(hdc, platform_left, AsConfig::Platform_Y_Pos * scale);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Expanding::Draw_Circle_Highlight(HDC hdc, int x, int y)
{
	const int scale = AsConfig::Global_Scale;

	Highlight_Color->Select_Pen(hdc);

	Arc(hdc, x + scale, y + scale, x + (AsConfig::Platform_Height - 1) * scale - 1, y + (AsConfig::Platform_Height - 1) * scale - 1,
		x + 2 * scale, y + scale, x + scale, y + 3 * scale);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Expanding::Reset()
{
	Curr_Expanding_Width = AsConfig::Platform_Normal_Width;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Expanding::Draw_Expanding_Part(HDC hdc, int x, RECT &inner_rect, bool is_left)
{
	int arc_mid_x, arc_top, arc_low;
	const int scale = AsConfig::Global_Scale;
	const double d_scale = AsConfig::D_Global_Scale;
	int arc_width = 4 * scale;
	int arc_offset = 3 * scale + 1;
	RECT circle_rect{}, arc_rect{};

	Circle_Color->Select(hdc);

	if (! is_left)
		x += (int)( (Curr_Expanding_Width - (double)AsConfig::Platform_Circle_Diameter) * d_scale);

	circle_rect.left = x + 1;
	circle_rect.top = AsConfig::Platform_Y_Pos * scale;
	circle_rect.right = circle_rect.left + AsConfig::Platform_Circle_Diameter * scale - 1;
	circle_rect.bottom = circle_rect.top + AsConfig::Platform_Circle_Diameter * scale;

	AsTools::Ellipse(hdc, circle_rect, *Circle_Color);

	if (is_left)
		arc_mid_x = circle_rect.left + arc_offset + arc_width / 2;
	else
		arc_mid_x = circle_rect.right - arc_offset - arc_width / 2;

	arc_rect.left = arc_mid_x - arc_width / 2;
	arc_rect.top = (AsConfig::Platform_Y_Pos + 1) * scale + 1;
	arc_rect.right = arc_rect.left + arc_width;
	arc_rect.bottom = (AsConfig::Platform_Y_Pos - 1 + AsConfig::Platform_Circle_Diameter) * scale - 1;

	AsTools::Ellipse(hdc, arc_rect, AsConfig::BG_Color);

	if (is_left)
	{
		arc_top = arc_rect.top;
		arc_low = arc_rect.bottom;
	}
	else
	{
		arc_top = arc_rect.bottom;
		arc_low = arc_rect.top;
	}

	Truss_Color->Select(hdc);
	Arc(hdc, arc_rect.left, arc_rect.top, arc_rect.right - 1, arc_rect.bottom - 1, arc_mid_x, arc_top, arc_mid_x, arc_low - 1);

	int truss_top, truss_low;
	int truss_x;

	if (is_left)
	{
		truss_x = inner_rect.left - 1;
		truss_top = inner_rect.top + 1;
		truss_low = inner_rect.bottom - scale + 1;
	}
	else
	{
		truss_x = inner_rect.right;
		truss_top = inner_rect.bottom - scale + 1;
		truss_low = inner_rect.top + 1;
		arc_mid_x -= 1;
	}

	int truss_center_x = truss_x - (truss_x - arc_mid_x) / 2;

	if (! is_left)
		truss_center_x -= 1;

	MoveToEx(hdc, arc_mid_x, truss_top, 0);
	LineTo(hdc, truss_center_x, truss_low);
	LineTo(hdc, truss_x, truss_top);

	MoveToEx(hdc, truss_x, truss_low, 0);
	LineTo(hdc, truss_center_x, truss_top);
	LineTo(hdc, arc_mid_x, truss_low);
}
//------------------------------------------------------------------------------------------------------------
