#include "Gate.h"


// AGate
//------------------------------------------------------------------------------------------------------------
const double AGate::Max_Gap_Short_Height = 9.0;
const double AGate::Opening_Short_Step = Max_Gap_Short_Height / (double)(AsConfig::FPS / 2.0);
const double AGate::Max_Gap_Long_Height = 18.0;
const double AGate::Opening_Long_Step = Max_Gap_Long_Height / (double)(AsConfig::FPS);
//------------------------------------------------------------------------------------------------------------
AGate::AGate(int x_pos, int y_pos, int level_x, int level_y)
	: Gate_State(EGate_State::Closed), Gate_Transformation(EGate_Transformation::Unknown), Gate_Rect{},
	X_Pos(x_pos), Y_Pos(y_pos), Origin_Y_Pos(y_pos), Level_X(level_x), Level_Y(level_y), Gate_Open_Timer_Tick(0), Gap_Height(0.0)
{
	Gate_Rect.left = X_Pos * AsConfig::Global_Scale;
	Gate_Rect.top = (int)(Y_Pos * AsConfig::D_Global_Scale);
	Gate_Rect.right = Gate_Rect.left + Width * AsConfig::Global_Scale;
	Gate_Rect.bottom = Gate_Rect.top + Height * AsConfig::Global_Scale;
}
//------------------------------------------------------------------------------------------------------------
void AGate::Act()
{
	bool correct_pos;

	switch (Gate_State)
	{
	case EGate_State::Closed:
		return;

	case EGate_State::Short_Open:
		Act_Opening(true, correct_pos);
		break;

	case EGate_State::Long_Open:
		Act_Opening(false, correct_pos);

		if (correct_pos)
		{
			Y_Pos = Origin_Y_Pos - Gap_Height / 2.0;

			Gate_Rect.top = (int)round(Y_Pos * AsConfig::D_Global_Scale);
			Gate_Rect.bottom = (int)round( ( Origin_Y_Pos + (double)Height + Gap_Height / 2.0) * AsConfig::D_Global_Scale);
		}
		break;

	default:
		AsTools::Throw();
	}
}
//------------------------------------------------------------------------------------------------------------
void AGate::Clear(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if ( ! IntersectRect(&intersection_rect, &paint_area, &Gate_Rect) )
		return;

	AsTools::Rect(hdc, Gate_Rect, AsConfig::BG_Color);
}
//------------------------------------------------------------------------------------------------------------
void AGate::Draw(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if ( ! IntersectRect(&intersection_rect, &paint_area, &Gate_Rect) )
		return;

	Clear(hdc, paint_area);

	Draw_Part(hdc, true);
	Draw_Part(hdc, false);

	// молния
	if (Gate_State == EGate_State::Long_Open && 
		(Gate_Transformation == EGate_Transformation::Init || Gate_Transformation == EGate_Transformation::Finalize) )
		Draw_Charge(hdc);
}
//------------------------------------------------------------------------------------------------------------
bool AGate::Is_Finished()
{
	return false;
}
//------------------------------------------------------------------------------------------------------------
void AGate::Open_Gate(bool is_short_open)
{
	if (! (Gate_State == EGate_State::Closed && Gate_Transformation == EGate_Transformation::Unknown) )
		AsTools::Throw();

	if (is_short_open)
		Gate_State = EGate_State::Short_Open;
	else
		Gate_State = EGate_State::Long_Open;

	Gate_Transformation = EGate_Transformation::Init;
}
//------------------------------------------------------------------------------------------------------------
bool AGate::Is_Gate_Open() const
{
	if (Gate_State != EGate_State::Closed && Gate_Transformation == EGate_Transformation::Active)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
bool AGate::Is_Gate_Close() const
{
	if (Gate_State == EGate_State::Closed)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
void AGate::Get_Y_Area(int &gate_top_y, int &gate_low_y) const
{
	gate_top_y = Gate_Rect.top;
	gate_low_y = Gate_Rect.bottom;
}
//------------------------------------------------------------------------------------------------------------
void AGate::Get_Gate_Pos(int &gate_x_pos, int &gate_y_pos) const
{
	gate_x_pos = X_Pos;
	gate_y_pos = (int)Origin_Y_Pos;
}
//------------------------------------------------------------------------------------------------------------
void AGate::Act_Opening(bool is_short, bool &correct_pos)
{
	double max_gap_height;
	double opening_step;
	int gate_open_timeout;

	correct_pos = false;

	if (is_short)
	{
		max_gap_height = Max_Gap_Short_Height;
		opening_step = Opening_Short_Step;
		gate_open_timeout = AsConfig::Gate_Short_Open_Timeout;
	}
	else
	{
		max_gap_height = Max_Gap_Long_Height;
		opening_step = Opening_Long_Step;
		gate_open_timeout = AsConfig::Gate_Long_Open_Timeout;
	}

	switch (Gate_Transformation)
	{
	case EGate_Transformation::Init:
		if (Gap_Height < max_gap_height)
		{
			Gap_Height += opening_step;

			if (Gap_Height > max_gap_height)
				Gap_Height = max_gap_height;

			correct_pos = true;
		}
		else
		{
			Gap_Height = max_gap_height;
			Gate_Open_Timer_Tick = AsConfig::Current_Timer_Tick + gate_open_timeout;
			Gate_Transformation = EGate_Transformation::Active;
		}

		Redraw_Gate();
		break;


	case EGate_Transformation::Active:
		if (Gate_Open_Timer_Tick == AsConfig::Current_Timer_Tick)
			Gate_Transformation = EGate_Transformation::Finalize;
		break;


	case EGate_Transformation::Finalize:
		if (Gap_Height > 0)
		{
			Gap_Height -= opening_step;

			if (Gap_Height < 0.0)
				Gap_Height = 0.0;

			correct_pos = true;
		}
		else
		{
			Gap_Height = 0;
			Gate_Transformation = EGate_Transformation::Unknown;
			Gate_State = EGate_State::Closed;
		}

		Redraw_Gate();
		break;

	default:
		AsTools::Throw();
	}
}
//------------------------------------------------------------------------------------------------------------
void AGate::Draw_Cup(HDC hdc, bool is_top) const
{
	const int cup_height = 4;
	const int scale = AsConfig::Global_Scale;
	const double d_scale = AsConfig::D_Global_Scale;
	RECT clip_rect{};
	HRGN cup_region;

	clip_rect.left = X_Pos * scale;
	clip_rect.top = (int)round(Y_Pos * d_scale);
	clip_rect.right = clip_rect.left + Width / 2 * scale;
	clip_rect.bottom = clip_rect.top + cup_height * scale + scale;

	if (! is_top)
	{
		if (Gate_State == EGate_State::Long_Open)
			clip_rect.top = (int)round( ( Y_Pos + (double)Height + Gap_Height) * d_scale);
		else
			clip_rect.top = (int)round( ( Y_Pos + (double)Height) * d_scale);

		clip_rect.bottom = clip_rect.top - cup_height * scale - scale;
	}

	cup_region = CreateRectRgnIndirect(&clip_rect);

	// чаша левая часть
	SelectClipRgn(hdc, cup_region);
	AsTools::Round_Rect(hdc, 0, 1, Width, 2 * cup_height, AsConfig::White_Color, 3);
	AsTools::Round_Rect(hdc, 1, 2, Width - 1, 2 * cup_height - 1, AsConfig::Blue_Color);

	// чаша правая часть
	clip_rect.left += Width / 2 * scale;
	clip_rect.right += Width / 2 * scale;

	cup_region = CreateRectRgnIndirect(&clip_rect);
	SelectClipRgn(hdc, cup_region);

	AsTools::Round_Rect(hdc, 0, 1, Width, 2 * cup_height, AsConfig::Blue_Color, 3);

	SelectClipRgn(hdc, 0);

	AsTools::Rect(hdc, 0, 4, 5, 1, AsConfig::White_Color);  // блик внизу чаши
	AsTools::Rect(hdc, 4, 3, 1, 1, AsConfig::BG_Color);  // перфорация

	DeleteObject(cup_region);
}
//------------------------------------------------------------------------------------------------------------
void AGate::Draw_Part(HDC hdc, bool is_top)
{
	const int scale = AsConfig::Global_Scale;
	const double d_scale = AsConfig::D_Global_Scale;
	XFORM xform{}, old_xform;

	xform.eM11 = 1.0f;
	xform.eM12 = 0.0f;
	xform.eM21 = 0.0f;
	xform.eM22 = 1.0f;
	xform.eDx = (float)(X_Pos * scale);
	xform.eDy = (float)round(Y_Pos * d_scale);

	if (! is_top)
	{
		xform.eM22 = -xform.eM22;

		if (Gate_State == EGate_State::Long_Open)
			xform.eDy = (float)round( (Y_Pos + (double)Height + Gap_Height) * d_scale - 1.0);
		else
			xform.eDy = (float)round( (Y_Pos + (double)Height) * d_scale - 1.0);
	}

	GetWorldTransform(hdc, &old_xform);
	SetWorldTransform(hdc, &xform);

	// перемычка
	AsTools::Rect(hdc, 2, 0, 2, 1, AsConfig::Blue_Color);

	// чаши
	Draw_Cup(hdc, true);
	Draw_Cup(hdc, false);

	// ребра
	Draw_Edges(hdc);

	SetWorldTransform(hdc, &old_xform);
}
//------------------------------------------------------------------------------------------------------------
void AGate::Draw_Edges(HDC hdc)
{
	if (Gate_State == EGate_State::Long_Open)
		Draw_Long_Open_Edges(hdc);
	else
		Draw_Short_Open_Edges(hdc);
}
//------------------------------------------------------------------------------------------------------------
void AGate::Draw_Short_Open_Edges(HDC hdc)
{
	int i;
	int count;
	bool is_long_edge = false;

	double ratio = 1.0 - Gap_Height / Max_Gap_Short_Height;

	count = (int)( (double)Edges_Count * ratio);

	for (i = 0; i < count; i++)
	{
		Draw_One_Edge(hdc, i, is_long_edge);
		is_long_edge = !is_long_edge;
	}
}
//------------------------------------------------------------------------------------------------------------
void AGate::Draw_Long_Open_Edges(HDC hdc)
{
	int i;
	double ratio = Gap_Height / Max_Gap_Long_Height;
	bool is_long_edge = false;

	if (ratio < 0.3)
	{
		for (i = 0; i < 4; i++)
		{
			Draw_One_Edge(hdc, i, is_long_edge);
			is_long_edge = !is_long_edge;
		}

		Draw_Red_Edge(hdc, 4, false, false);
	}
	else if (ratio < 0.5)
	{
		Draw_One_Edge(hdc, 0, false);
		Draw_One_Edge(hdc, 1, true);

		Draw_Red_Edge(hdc, 2, true, true);
		Draw_Red_Edge(hdc, 3, false, false);
	}
	else
	{
		Draw_One_Edge(hdc, 0, true);

		Draw_Red_Edge(hdc, 1, true, false);
		Draw_Red_Edge(hdc, 2, true, true);
		Draw_Red_Edge(hdc, 3, false, false);

	}
}
//------------------------------------------------------------------------------------------------------------
void AGate::Draw_Red_Edge(HDC hdc, int edges_y_offset, bool is_long_edge, bool has_highlight)
{
	int edges_y_pos = 5;

	if (is_long_edge)
	{
		AsTools::Rect(hdc, 0, edges_y_pos + edges_y_offset, 6, 1, AsConfig::Red_Color);

		if (has_highlight)
			AsTools::Rect(hdc, 1, edges_y_pos + edges_y_offset, 1, 1, AsConfig::White_Color);
	}
	else
		AsTools::Rect(hdc, 1, edges_y_pos + edges_y_offset, 4, 1, AsConfig::Red_Color);
}
//------------------------------------------------------------------------------------------------------------
void AGate::Draw_One_Edge(HDC hdc, int edges_y_offset, bool is_long_edge)
{
	int edges_y_pos = 5;

	if (is_long_edge)
		AsTools::Rect(hdc, 0, edges_y_pos + edges_y_offset, 6, 1, AsConfig::White_Color);
	else
	{
		AsTools::Rect(hdc, 1, edges_y_pos + edges_y_offset, 2, 1, AsConfig::Blue_Color);
		AsTools::Rect(hdc, 4, edges_y_pos + edges_y_offset, 1, 1, AsConfig::Blue_Color);
	}
}
//------------------------------------------------------------------------------------------------------------
void AGate::Draw_Charge(HDC hdc) const
{
	int i;
	int dot_x, dot_y;
	int field_y;
	double ratio = Gap_Height / Max_Gap_Long_Height;

	if (ratio < 0.2 || ratio > 0.9)
		return;

	field_y = (int)(Origin_Y_Pos + (double)Height / 2.0 - Gap_Height / 2.0);

	for (i = 0; i < 4; i++)
	{
		dot_x = 1 + AsTools::Rand(4);
		dot_y = AsTools::Rand( (int)Gap_Height);

		AsTools::Rect(hdc, X_Pos + dot_x, field_y + dot_y, 1, 1, AsConfig::Blue_Color);
	}
}
//------------------------------------------------------------------------------------------------------------
void AGate::Redraw_Gate()
{
	--Gate_Rect.top;
	++Gate_Rect.bottom;

	AsTools::Invalidate_Rect(Gate_Rect);

	++Gate_Rect.top;
	--Gate_Rect.bottom;
}
//------------------------------------------------------------------------------------------------------------
