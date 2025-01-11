#include "Platform_Laser.h"

// AsPlatform_Laser
//------------------------------------------------------------------------------------------------------------
AsPlatform_Laser::~AsPlatform_Laser()
{
	delete Gun_Color;
}
//------------------------------------------------------------------------------------------------------------
AsPlatform_Laser::AsPlatform_Laser(AsPlatform_State &platform_state)
	: Platform_State(&platform_state), Gun_Color(0), Inner_Color(0), Circle_Color(0), On_Fire(false),
	Laser_Beams_Set(0), Laser_Finalize_Timer_Tick(0), Laser_Fire_Timer_Tick(0), Laser_Transformation_Step(0)
{
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Laser::Init(AColor &inner_color, AColor &circle_color, AsLaser_Beam_Set *laser_beams_set)
{
	Inner_Color = &inner_color;
	Circle_Color = &circle_color;

	Laser_Beams_Set = laser_beams_set;

	Gun_Color = new AColor(AsConfig::White_Color, AsConfig::Global_Scale);
}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform_Laser::Act(double platform_x_pos, EPlatform_State &next_state)
{
	double left_gun_x_pos, right_gun_x_pos;
	next_state = EPlatform_State::Unknown;

	switch (Platform_State->Laser)
	{
	case EPlatform_Transformation::Init:
		if (Laser_Transformation_Step < Max_Laser_Transformation_Step)
			++Laser_Transformation_Step;
		else
		{
			Platform_State->Laser = EPlatform_Transformation::Active;
			Laser_Finalize_Timer_Tick = AsConfig::Current_Timer_Tick + AsConfig::Platform_Laser_Timeout;
		}
		return true;


	case EPlatform_Transformation::Active:
		if (AsConfig::Current_Timer_Tick == Laser_Finalize_Timer_Tick)
		{
			Platform_State->Laser = EPlatform_Transformation::Finalize;
			return false;
		}

		if (On_Fire)
		{
			if (Laser_Fire_Timer_Tick <= AsConfig::Current_Timer_Tick)
			{
				Laser_Fire_Timer_Tick = AsConfig::Current_Timer_Tick + AsConfig::Laser_Fire_Timeout;

				left_gun_x_pos = Get_Gun_Pos(platform_x_pos, true);
				right_gun_x_pos = Get_Gun_Pos(platform_x_pos, false);

				if (Laser_Beams_Set != 0)
					Laser_Beams_Set->Fire(left_gun_x_pos, right_gun_x_pos);
			}
		}
		break;


	case EPlatform_Transformation::Finalize:
		if (Laser_Transformation_Step > 0)
			--Laser_Transformation_Step;
		else
		{
			Laser_Transformation_Step = 0;
			Platform_State->Laser = EPlatform_Transformation::Unknown;
			next_state = Platform_State->Set_State(EPlatform_Substate_Regular::Normal);
			On_Fire = false;
		}
		return true;

	default:
		AsTools::Throw();
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Laser::Draw_State(HDC hdc, double x_pos, RECT &curr_rect)
{
	HRGN region;

	region = CreateRectRgnIndirect(&curr_rect);
	SelectClipRgn(hdc, region);

	// крылья
	Draw_Laser_Wing(hdc, x_pos, true);
	Draw_Laser_Wing(hdc, x_pos, false);

	// центральная часть
	Draw_Laser_Leg(hdc, x_pos, true);
	Draw_Laser_Leg(hdc, x_pos, false);

	// кабина 
	Draw_Laser_Cabin(hdc, x_pos);

	SelectClipRgn(hdc, 0);
	DeleteObject(region);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Laser::Fire(bool on_fire)
{
	if (Platform_State->Laser != EPlatform_Transformation::Active)
		return;

	On_Fire = on_fire;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Laser::Draw_Laser_Wing(HDC hdc, double x_pos, bool is_left)
{
	int half_max_step;
	double gun_height;
	double x_1, y_1, y_2;
	double right_part_x = 0.0;
	double height_1, height_2;
	double width_1, width_2;
	const double d_scale = AsConfig::D_Global_Scale;
	double ratio = (double)Laser_Transformation_Step / (double)Max_Laser_Transformation_Step;

	// 1.0 крыло
	Circle_Color->Select(hdc);

	// 1.1 ширина эллипса и координата х не меняются
	width_1 = (double)AsConfig::Platform_Circle_Diameter;

	if (is_left)
		x_1 = x_pos;
	else
		x_1 = x_pos + (double)AsConfig::Platform_Normal_Width - (double)AsConfig::Platform_Circle_Diameter;

	// 1.2 координата у эллипса крыла изменяется [AsConfig::Platform_Y_Pos (в нормальном состоянии) ... y_1 + 1.0]
	y_1 = (double)AsConfig::Platform_Y_Pos;
	y_2 = y_1 + 1.0;

	// 1.3 высота эллипса крыла изменяется [AsConfig::Platform_Circle_Diameter (в нормальном состоянии) ... 2 * AsConfig::Platform_Circle_Diameter]
	height_1 = (double)AsConfig::Platform_Circle_Diameter;
	height_2 = 2.0 * height_1;

	Draw_Expanding_Figure(hdc, EFigure_Type::Ellipse, x_1, y_1, width_1, height_1, ratio, x_1, y_2, width_1, height_2);

	// 2.0 перемычка крыла
	// 2.1 координата х и у не меняются	
	if (is_left)
		x_1 = x_pos + 5.0;  // левая
	else  // правая
		x_1 = x_pos + (double)AsConfig::Platform_Normal_Width - 5.0 - 1.0 / d_scale;

	y_1 = (double)AsConfig::Platform_Y_Pos + 2.0;

	// 2.2 ширина прямоугольника ьеняется [ 0.0 ... width_2 ]
	if (is_left)
		width_2 = 6.0;
	else
		width_2 = -5.0 - 1.0 / d_scale;

	// 2.2 высота прямоугольника меняется [ 0.0 ... (double)AsConfig::Platform_Circle_Diameter - 2.0 ]
	height_2 = (double)AsConfig::Platform_Circle_Diameter - 2.0;

	Draw_Expanding_Figure(hdc, EFigure_Type::Rectangle, x_1, y_1, 0.0, 0.0, ratio, x_1, y_1, width_2, height_2);

	// 3.0 лазеры
	Gun_Color->Select_Pen(hdc);
	half_max_step = Max_Laser_Transformation_Step / 2;

	if (Laser_Transformation_Step >= half_max_step)
	{
		ratio = (double)(Laser_Transformation_Step - half_max_step) / (double)half_max_step;

		x_1 = Get_Gun_Pos(x_pos, is_left);

		y_1 = (double)AsConfig::Platform_Y_Pos + 3.0;
		gun_height = 3.0 * d_scale;

		MoveToEx(hdc, (int)(x_1 * d_scale + 1.0), (int)(y_1 * d_scale) + 1, 0);
		LineTo(hdc, (int)(x_1 * d_scale + 1.0), (int)(y_1 * d_scale - ratio * gun_height) + 1);

		Draw_Expanding_Figure(hdc, EFigure_Type::Ellipse, x_1 + 1.0, y_1 + 3.0, 0.0, 0.0, ratio, x_1 - 1, y_1 + 2.0, 3.0, 6.0);
	}
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Laser::Draw_Laser_Leg(HDC hdc, double x_pos, bool is_left)
{
	double x, y;
	double x_scale;
	const double d_scale = AsConfig::D_Global_Scale;
	double ratio = (double)Laser_Transformation_Step / (double)Max_Laser_Transformation_Step;

	Inner_Color->Select(hdc);

	if (is_left)
	{
		x = (x_pos + 6.0) * d_scale;
		x_scale = d_scale;
	}
	else
	{
		x = x_pos * d_scale + (double)(AsConfig::Platform_Normal_Width - 6) * d_scale - 1.0;
		x_scale = -d_scale;
	}

	y = (double)(AsConfig::Platform_Y_Pos + 3) * d_scale;

	POINT left_leg_points[7] =
	{
		{ (int)x, (int)y }, { (int)(x + 2.0 * x_scale), (int)(y - 2 * d_scale) },
		{ (int)(x + 4 * x_scale), (int)(y - 2 * d_scale) }, { (int)(x + 4 * x_scale), (int)y },
		{ (int)(x + 2 * x_scale), (int)(y + 2 * d_scale) }, { (int)(x + 2 * x_scale), (int)(y + (2.0 + 2.0 * ratio) * d_scale) },
		{ (int)x, (int)(y + 4.0 * ratio * d_scale) } 
	};

	Polygon(hdc, left_leg_points, 7);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Laser::Draw_Laser_Cabin(HDC hdc, double x_pos)
{
	double x, y;
	double x_center;
	double ratio = (double)Laser_Transformation_Step / (double)Max_Laser_Transformation_Step;

	Inner_Color->Select(hdc);

	// Размер: [ 20 x 5 ... 8 x 1 ]
	x = x_pos;
	x_center = x_pos + (double)AsConfig::Platform_Normal_Width / 2.0;
	y = AsConfig::Platform_Y_Pos;

	Inner_Color->Select(hdc);
	Draw_Expanding_Figure(hdc, EFigure_Type::Round_Rect_3x, x + 4, y + 1, 20, 5, ratio, x + 10, y + 3, 8, 1);

	Draw_Expanding_Figure(hdc, EFigure_Type::Ellipse, x_center, y + 2, 0, 0, ratio, x + 7, y - 1, 14, 7);

	AsConfig::BG_Color.Select(hdc);
	Draw_Expanding_Figure(hdc, EFigure_Type::Ellipse, x_center, y + 2, 0, 0, ratio, x + 9, y - 1, 10, 5);

	AsConfig::White_Color.Select(hdc);
	Draw_Expanding_Figure(hdc, EFigure_Type::Ellipse, x_center, y + 2, 0, 0, ratio, x + 10, y, 8, 3);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Laser::Draw_Expanding_Figure(HDC hdc, EFigure_Type figure_type, double start_x, double start_y, double start_width, double start_height, double ratio, double end_x, double end_y, double end_width, double end_height)
{
	double x, y;
	double width, height;
	const double d_scale = AsConfig::D_Global_Scale;
	RECT rect{};

	x = Get_Expanding_Value( start_x, end_x, ratio);
	y = Get_Expanding_Value( start_y, end_y, ratio);
	width = Get_Expanding_Value( start_width, end_width, ratio);
	height = Get_Expanding_Value( start_height, end_height, ratio);

	rect.left = (int)(x * d_scale);
	rect.top = (int)(y * d_scale);
	rect.right = (int)( (x + width) * d_scale);
	rect.bottom = (int)( (y + height) * d_scale);

	switch (figure_type)
	{
	case EFigure_Type::Ellipse:
		Ellipse(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1);
		break;

	case EFigure_Type::Rectangle:
		Rectangle(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1);
		break;

	case EFigure_Type::Round_Rect_3x:
		AsTools::Round_Rect(hdc, rect, 3);
		break;

	default:
		AsTools::Throw();
	}
}
//------------------------------------------------------------------------------------------------------------
double AsPlatform_Laser::Get_Expanding_Value(double start_value, double end_value, double ratio)
{
	double expanding_value;

	expanding_value = start_value + (end_value - start_value) * ratio;

	return expanding_value;
}
//------------------------------------------------------------------------------------------------------------
double AsPlatform_Laser::Get_Gun_Pos(double platform_x_pos, bool is_left)
{
	if (is_left)
		return platform_x_pos + 3.0;
	else
		return platform_x_pos + (double)AsConfig::Platform_Normal_Width - 4.0;
}
//------------------------------------------------------------------------------------------------------------
