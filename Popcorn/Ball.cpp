#include "Ball.h"

// ABall
AHit_Checker_List ABall::Hit_Checker_List;
//------------------------------------------------------------------------------------------------------------
ABall::ABall()
	: Ball_State(EBall_State::Disabled), Prev_Ball_State(EBall_State::Lost), Release_Timer_Tick(0), Center_X_Pos(0.0), Center_Y_Pos(0.0), Ball_Speed(0.0), Prev_Ball_Speed(0.0),
	Ball_Direction(0.0), Prev_Ball_Direction(0.0), Testing_Is_Active(false), Rest_Test_Distance(0.0), Test_Iteration(0), 
	Prev_Ball_Rect{}, Ball_Rect{}, Parashute_Rect{}, Prev_Parashute_Rect{}
{}
//------------------------------------------------------------------------------------------------------------
void ABall::Begin_Movement()
{
	if (Ball_State == EBall_State::Disabled || Ball_State == EBall_State::On_Platform)
		return;

	Prev_Ball_Rect = Ball_Rect;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Finish_Movement()
{
	if (Ball_State == EBall_State::Disabled || Ball_State == EBall_State::On_Platform)
		return;

	Redraw_Ball();

	if (Ball_State == EBall_State::On_Parashute)
	{
		Prev_Parashute_Rect = Parashute_Rect;

		Parashute_Rect.bottom = Ball_Rect.bottom;
		Parashute_Rect.top = Parashute_Rect.bottom - Parashute_Size * AsConfig::Global_Scale;

		Redraw_Parashute();
	}
}
//------------------------------------------------------------------------------------------------------------
void ABall::Advance(double max_speed)
{
	bool got_hit = true;
	double next_step;
	double next_x_pos, next_y_pos;

	if (! (Ball_State == EBall_State::Normal || Ball_State == EBall_State::On_Parashute) )
		return;

	while (got_hit)
	{
		got_hit = false;

		next_step = Ball_Speed / max_speed * AsConfig::Moving_Step_Size;

		next_x_pos = Center_X_Pos + next_step * cos(Ball_Direction);
		next_y_pos = Center_Y_Pos - next_step * sin(Ball_Direction);

		got_hit = Hit_Checker_List.Check_Hit(next_x_pos, next_y_pos, this);		

		if (! got_hit)
		{
			if (Ball_State == EBall_State::On_Glue_Platform)
				return;

			Center_X_Pos = next_x_pos;
			Center_Y_Pos = next_y_pos;

			if (Testing_Is_Active)
				Rest_Test_Distance -= AsConfig::Moving_Step_Size;
		}
	}
}
//------------------------------------------------------------------------------------------------------------
double ABall::Get_Speed()
{
	return Ball_Speed;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Act()
{
	// Заглушка. Этот метод не используется, т.к. у мячика нету анимации
}
//------------------------------------------------------------------------------------------------------------
void ABall::Clear(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if (Ball_State == EBall_State::Disabled)
		return;

	if (IntersectRect(&intersection_rect, &paint_area, &Prev_Ball_Rect) )
		AsTools::Ellipse(hdc, Prev_Ball_Rect, AsConfig::BG_Color);
}
//------------------------------------------------------------------------------------------------------------
void ABall::Draw(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if (Ball_State == EBall_State::Disabled)
		return;

	if (Ball_State == EBall_State::Teleporting_Stage_1 || Ball_State == EBall_State::Teleporting_Stage_2)
		return;

	switch(Ball_State)
	{
	case EBall_State::On_Parashute:
		Draw_Parashute(hdc, paint_area);
		break;

	case EBall_State::Off_Parashute:
		Clear_Parashute(hdc);
		Set_State(EBall_State::Normal, Center_X_Pos, Center_Y_Pos);
		break;

	case EBall_State::Lost:
		if (Prev_Ball_State == EBall_State::On_Parashute)
			Clear_Parashute(hdc);
		return;
	}

	if (IntersectRect(&intersection_rect, &paint_area, &Ball_Rect) )
		AsTools::Ellipse(hdc, Ball_Rect, AsConfig::White_Color);
}
//------------------------------------------------------------------------------------------------------------
bool ABall::Is_Finished()
{
	return false;  // Заглушка. Этот метод не используется
}
//------------------------------------------------------------------------------------------------------------
double ABall::Get_Direction()
{
	return Ball_Direction;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Set_Direction(double new_direction)
{
	double full_angle = 2.0 * M_PI;

	// 1. Переводим угол в диапазон [0 .. 2.0 * M_PI]
	while (new_direction > full_angle)
		new_direction -= full_angle;

	while (new_direction < 0.0)
		new_direction += full_angle;

	// 2. Не позволим приближаться к горизонтальной оси ближе, чем на угол AsConfig::Min_Ball_Angle
	// 2.1. Слева
	// 2.1.1. Сверху
	if (new_direction < AsConfig::Min_Ball_Angle)
		new_direction = AsConfig::Min_Ball_Angle;

	// 2.1.1. Снизу
	if (new_direction > full_angle - AsConfig::Min_Ball_Angle)
		new_direction = full_angle - AsConfig::Min_Ball_Angle;

	// 2.2. Справа
	// 2.2.1. Сверху
	if (new_direction > M_PI - AsConfig::Min_Ball_Angle && new_direction < M_PI)
		new_direction = M_PI - AsConfig::Min_Ball_Angle;

	// 2.2.1. Снизу
	if (new_direction >= M_PI && new_direction < M_PI + AsConfig::Min_Ball_Angle)
		new_direction = M_PI + AsConfig::Min_Ball_Angle;

	Ball_Direction = new_direction;
}
//------------------------------------------------------------------------------------------------------------
EBall_State ABall::Get_State()
{
	return Ball_State;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Set_State(EBall_State new_state, double x_pos, double y_pos)
{
	switch (new_state)
	{
	case EBall_State::Disabled:
		Ball_Speed = 0.0;
		break;


	case EBall_State::Normal:
		if (Ball_State == EBall_State::Disabled || Ball_State == EBall_State::Lost)
		{
			Center_X_Pos = x_pos;
			Center_Y_Pos = y_pos;
			Ball_Speed = AsConfig::Ball_Normal_Speed;
			Ball_Direction = M_PI_4;
		}

		Redraw_Ball();
		break;


	case EBall_State::Lost:
		if (! (Ball_State == EBall_State::On_Parashute || Ball_State == EBall_State::Normal) )
			AsTools::Throw();

		Ball_Speed = 0.0;
		Redraw_Ball();
		Redraw_Parashute();
		break;


	case EBall_State::On_Platform:
		Center_X_Pos = x_pos;
		Center_Y_Pos = y_pos;
		Ball_Speed = AsConfig::Ball_Normal_Speed;
		Ball_Direction = M_PI_4;
		Redraw_Ball();
		break;


	case EBall_State::On_Glue_Platform:
		Release_Timer_Tick = AsConfig::Current_Timer_Tick + AsConfig::Ball_On_Glue_Timeout;
		break;


	case EBall_State::On_Parashute:
		AsTools::Throw();
		break;


	case EBall_State::Off_Parashute:
		if (Ball_State != EBall_State::On_Parashute)
			AsTools::Throw();

		Ball_Speed = 0.0;
		Redraw_Ball();
		Redraw_Parashute();
		break;


	case EBall_State::Teleporting_Stage_1:
		if (! (Ball_State == EBall_State::On_Parashute || Ball_State == EBall_State::Normal) )
			AsTools::Throw();

		Center_X_Pos = x_pos;
		Center_Y_Pos = y_pos;
		Ball_Speed = 0.0;
		Redraw_Ball();

		if (Prev_Ball_State == EBall_State::On_Parashute)
			Redraw_Parashute();
		break;


	case EBall_State::Teleporting_Stage_2:
		if (Ball_State != EBall_State::Teleporting_Stage_1)
			AsTools::Throw();

		Center_X_Pos = x_pos;
		Center_Y_Pos = y_pos;
		Ball_Speed = 0.0;
		Redraw_Ball();

		break;


	default:
		AsTools::Throw();
	}

	Prev_Ball_State = Ball_State;
	Ball_State = new_state;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Reflect(bool from_horizontal)
{
	if (from_horizontal)
		Set_Direction(-Ball_Direction);
	else
		Set_Direction(M_PI - Ball_Direction);
}
//------------------------------------------------------------------------------------------------------------
bool ABall::Is_Moving_Up()
{
	if (Ball_Direction >= 0.0 && Ball_Direction < M_PI)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
bool ABall::Is_Moving_Left()
{
	if (Ball_Direction > M_PI_2 && Ball_Direction < M_PI + M_PI_2)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Set_On_Parachute(int brick_y, int brick_x)
{
	int cell_x, cell_y;

	cell_x = AsConfig::Bricks_Area_X_Offset + brick_x * AsConfig::Cell_Width;
	cell_y = AsConfig::Bricks_Area_Y_Offset + brick_y * AsConfig::Cell_Height;

	Parashute_Rect.left = cell_x * AsConfig::Global_Scale;
	Parashute_Rect.top = cell_y * AsConfig::Global_Scale;
	Parashute_Rect.right = Parashute_Rect.left + Parashute_Size * AsConfig::Global_Scale;
	Parashute_Rect.bottom = Parashute_Rect.top + Parashute_Size * AsConfig::Global_Scale;

	Prev_Parashute_Rect = Parashute_Rect;

	Redraw_Parashute();

	Center_X_Pos = (double)cell_x + (double)Parashute_Size / 2.0;
	Center_Y_Pos = (double)(cell_y + Parashute_Size) - AsConfig::Ball_Radius;

	Ball_Direction = M_PI + M_PI_2;
	Ball_Speed = 1.0;

	Ball_State = EBall_State::On_Parashute;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Get_Center(double &x_pos, double &y_pos)
{
	x_pos = Center_X_Pos;
	y_pos = Center_Y_Pos;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Draw_Teleporting(HDC hdc, int step)
{
	int top = Ball_Rect.top + step;
	int bottom_y = Ball_Rect.bottom - step - 1;

	if (top > bottom_y)
		return;

	AsConfig::White_Color.Select(hdc);
	Ellipse(hdc, Ball_Rect.left, top, Ball_Rect.right - 1, bottom_y);
}
//------------------------------------------------------------------------------------------------------------
void ABall::Set_Speed(double new_speed)
{
	if (new_speed > AsConfig::Max_Ball_Speed)
		new_speed = AsConfig::Max_Ball_Speed;

	Ball_Speed = new_speed;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Set_For_Test()
{
	Testing_Is_Active = true;
	Rest_Test_Distance = 30.0;

	Set_State(EBall_State::Normal, 90 + Test_Iteration, 90);
	Ball_Direction = M_PI - M_PI_4;

	++Test_Iteration;
}
//------------------------------------------------------------------------------------------------------------
bool ABall::Is_Test_Finished()
{
	if (Testing_Is_Active)
	{
		if (Rest_Test_Distance <= 0.0)
		{
			Testing_Is_Active = false;
			Set_State(EBall_State::Lost);
			return true;
		}
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Forced_Advance(double offset)
{
	Center_X_Pos += offset;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Redraw_Ball()
{
	Ball_Rect.left = (int)( (Center_X_Pos - AsConfig::Ball_Radius) * AsConfig::D_Global_Scale);
	Ball_Rect.top = (int)( (Center_Y_Pos - AsConfig::Ball_Radius) * AsConfig::D_Global_Scale);
	Ball_Rect.right = (int)( (Center_X_Pos + AsConfig::Ball_Radius) * AsConfig::D_Global_Scale);
	Ball_Rect.bottom = (int)( (Center_Y_Pos + AsConfig::Ball_Radius) * AsConfig::D_Global_Scale);

	AsTools::Invalidate_Rect(Prev_Ball_Rect);
	AsTools::Invalidate_Rect(Ball_Rect);
}
//------------------------------------------------------------------------------------------------------------
void ABall::Redraw_Parashute()
{
	AsTools::Invalidate_Rect(Prev_Parashute_Rect);
	AsTools::Invalidate_Rect(Parashute_Rect);
}
//------------------------------------------------------------------------------------------------------------
void ABall::Draw_Parashute(HDC hdc, RECT &paint_area)
{
	const int scale = AsConfig::Global_Scale;
	int dome_height = (Parashute_Rect.bottom - Parashute_Rect.top) / 2;
	int arc_height = 4 * scale;
	int ball_x, ball_y;
	RECT intersection_rect, arc{}, other_arc;

	if (! IntersectRect(&intersection_rect, &paint_area, &Parashute_Rect) )
		return;

	Clear_Parashute(hdc);

	if (!(Ball_State == EBall_State::On_Parashute) )
		return;

	// main dome
	AsConfig::Blue_Color.Select(hdc);

	Chord(hdc, Parashute_Rect.left, Parashute_Rect.top, Parashute_Rect.right - 1, Parashute_Rect.bottom - 3 * scale - 1,
		Parashute_Rect.right - 1, Parashute_Rect.top + dome_height, Parashute_Rect.left, Parashute_Rect.top + dome_height);

	// arcs
	arc.left = Parashute_Rect.left + 1;
	arc.top = Parashute_Rect.top + dome_height - arc_height / 2 - 1;
	arc.right = arc.left + 3 * scale;
	arc.bottom = arc.top + arc_height;

	AsTools::Ellipse(hdc, arc, AsConfig::BG_Color);  // left arc

	other_arc = arc;
	other_arc.left = arc.right + scale - 1;
	other_arc.right = other_arc.right + 7 * scale + 1;

	AsTools::Ellipse(hdc, other_arc, AsConfig::BG_Color);  // middle arc

	other_arc.left = other_arc.right + scale - 1;
	other_arc.right = other_arc.left + 3 * scale;

	AsTools::Ellipse(hdc, other_arc, AsConfig::BG_Color);  // right arc

	// slings
	ball_x = (int)Center_X_Pos * scale;
	ball_y = (int)Center_Y_Pos * scale;

	AsConfig::White_Color.Select_Pen(hdc);

	MoveToEx(hdc, ball_x, ball_y, 0);
	LineTo(hdc, Parashute_Rect.left, Parashute_Rect.top + dome_height - scale);

	MoveToEx(hdc, ball_x, ball_y, 0);
	LineTo(hdc, Parashute_Rect.right - 1, Parashute_Rect.top + dome_height - scale);

	MoveToEx(hdc, ball_x, ball_y, 0);
	LineTo(hdc, arc.right + 1, Parashute_Rect.top + dome_height);

	MoveToEx(hdc, ball_x, ball_y, 0);
	LineTo(hdc, other_arc.left - 1, Parashute_Rect.top + dome_height);
}
//------------------------------------------------------------------------------------------------------------
void ABall::Clear_Parashute(HDC hdc)
{
	AsConfig::BG_Color.Select(hdc);
	AsTools::Round_Rect(hdc, Prev_Parashute_Rect, 0);
}
//------------------------------------------------------------------------------------------------------------
