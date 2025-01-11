#include "Platform.h"

// AsPlatform
AHit_Checker_List AsPlatform::Hit_Checker_List;
const double AsPlatform::Platform_Normal_Speed = 3.0;
//------------------------------------------------------------------------------------------------------------
AsPlatform::AsPlatform()
	: Platform_Glue(Platform_State), Platform_Expanding(Platform_State), Platform_Laser(Platform_State), Platform_State(), Ball_Set(0),
	Prev_X_Pos(0.0), X_Pos(101.0), Left_Key_Down(false), Right_Key_Down(false), Inner_Width(Normal_Inner_Width),
	Rolling_Step(0), Current_Redraw_Timer_Tick(0),
	Circle_Color(255, 85, 255), Inner_Color(85, 255, 255),
	Highlight_Color(255, 255, 255), Prev_Rect{}, Curr_Rect{}
{}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform::Check_Hit(double next_x_pos, double next_y_pos, ABall_Object *ball)
{
	double inner_left_x, inner_right_x;
	double inner_top_y, inner_low_y;
	double reflection_pos;
	double circle_x, circle_y;
	double circle_radius;
	double right_circle_x_offset;

	if (next_y_pos + AsConfig::Ball_Radius < AsConfig::Platform_Y_Pos)
		return false;

	circle_radius = (double)AsConfig::Platform_Circle_Diameter / 2.0;
	circle_x = (double)X_Pos + circle_radius;
	circle_y = (double)AsConfig::Platform_Y_Pos + circle_radius;

	// Проверка столкновения с шариком платформы
	if(AsTools::Reflect_From_Circle(next_x_pos, next_y_pos, circle_radius, circle_x, circle_y, ball) )
		goto _on_hit_exit;  // Левый

	right_circle_x_offset = Get_Current_Width() - AsConfig::Platform_Circle_Diameter;
	if(AsTools::Reflect_From_Circle(next_x_pos, next_y_pos, circle_radius, circle_x + right_circle_x_offset, circle_y, ball) )
		goto _on_hit_exit;  // Правый

	inner_top_y = AsConfig::Platform_Y_Pos + 1;
	inner_low_y = AsConfig::Platform_Y_Pos - 1 + AsConfig::Platform_Circle_Diameter;

	inner_left_x = X_Pos + (double)AsConfig::Platform_Circle_Diameter - 1.0;
	inner_right_x = X_Pos + (double)(Get_Current_Width() - AsConfig::Platform_Circle_Diameter + 1);

	if (Hit_Circle_On_Line(next_y_pos - inner_top_y, next_x_pos, inner_left_x, inner_right_x, AsConfig::Ball_Radius, reflection_pos) )
	{
		if (Platform_State == EPlatform_State::Glue && Platform_State.Glue == EPlatform_Transformation::Active)
		{
			ball->Reflect(true);
			ball->Set_State(EBall_State::On_Glue_Platform);
			return false;
		}

		ball->Reflect(true);
		goto _on_hit_exit;
	}
	else if (Hit_Circle_On_Line(next_y_pos - inner_low_y, next_x_pos, inner_left_x, inner_right_x, AsConfig::Ball_Radius, reflection_pos) )
	{
		ball->Reflect(true);
		goto _on_hit_exit;
	}
	return false;

_on_hit_exit:
	if (ball->Get_State() == EBall_State::On_Parashute)
		ball->Set_State(EBall_State::Off_Parashute);

	return true;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Begin_Movement()
{
	// Заглушка. Этот метод не используется
}
//-----------------------------------------------------------------------------------------------------------
void AsPlatform::Finish_Movement()
{
	if (Platform_State.Moving == EPlatform_Moving_State::Stop)
		return;

	Redraw();

	if (Platform_State.Moving == EPlatform_Moving_State::Stopping)
		Platform_State.Moving = EPlatform_Moving_State::Stop;
}
//-----------------------------------------------------------------------------------------------------------
void AsPlatform::Advance(double max_speed)
{
	double next_step;

	if (Platform_State.Moving == EPlatform_Moving_State::Stopping || Platform_State.Moving == EPlatform_Moving_State::Stop)
		return;

	next_step = Speed / max_speed * AsConfig::Moving_Step_Size;

	Prev_X_Pos = X_Pos;

	X_Pos += next_step;

	if (Correct_Platform_Pos() )
	{
		Speed = 0.0;
		Platform_State.Moving = EPlatform_Moving_State::Stopping;
	}

	Hit_Checker_List.Check_Hit(Curr_Rect);
}
//-----------------------------------------------------------------------------------------------------------
bool AsPlatform::Correct_Platform_Pos()
{
	double platform_max_x_pos = (double)AsConfig::Level_Max_X_Offset - Get_Current_Width();

	if (X_Pos < AsConfig::Level_X_Offset)
	{
		X_Pos = (double)AsConfig::Level_X_Offset;
		return true;
	}
	else if (X_Pos > platform_max_x_pos)
	{
		X_Pos = platform_max_x_pos;
		return true;
	}
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
double AsPlatform::Get_Speed()
{
	return Speed;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Act()
{
	EPlatform_State next_state;

	switch(Platform_State)
	{
	case EPlatform_State::Rolling:
		Act_Rolling_State();
		break;

	case EPlatform_State::Meltdown:
		Act_Meltdown_State();
		break;

	case EPlatform_State::Glue:
		if (Platform_Glue.Act(Ball_Set, next_state) )
			Redraw();

		if (next_state != EPlatform_State::Unknown)
			Set_State(next_state);
		break;

	case EPlatform_State::Expanding:
		if (Platform_Expanding.Act(X_Pos, next_state) )
		{
			Correct_Platform_Pos();
			Redraw();
		}

		if (next_state != EPlatform_State::Unknown)
			Set_State(next_state);
		break;

	case EPlatform_State::Laser:
		if (Platform_Laser.Act(X_Pos, next_state) )
			Redraw();

		if (next_state != EPlatform_State::Unknown)
			Set_State(next_state);
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Clear(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if ( ! IntersectRect(&intersection_rect, &paint_area, &Prev_Rect) )
		return;

	switch (Platform_State)
	{
	case EPlatform_State::Regular:
	{
		if (Platform_State.Regular == EPlatform_Substate_Regular::Missing)
			break;
		// else no break!
	}
	case EPlatform_State::Rolling:
	case EPlatform_State::Meltdown:
	case EPlatform_State::Glue:
	case EPlatform_State::Expanding:
	case EPlatform_State::Laser:
		AsTools::Rect(hdc, Prev_Rect, AsConfig::BG_Color);
	}
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if ( ! IntersectRect(&intersection_rect, &paint_area, &Curr_Rect) )
		return;

	switch (Platform_State)
	{
	case EPlatform_State::Rolling:
		Draw_Rolling_State(hdc);
		break;

	case EPlatform_State::Regular:
		if (Platform_State.Regular == EPlatform_Substate_Regular::Ready || Platform_State.Regular == EPlatform_Substate_Regular::Normal)
			Draw_Normal_State(hdc);
		break;

	case EPlatform_State::Meltdown:
		if (Platform_State.Meltdown == EPlatform_Substate_Meltdown::Active)
			Draw_Meltdown_State(hdc, paint_area);
		break;

	case EPlatform_State::Glue:
		Draw_Normal_State(hdc);
		Platform_Glue.Draw_State(hdc, (int)X_Pos);
		break;

	case EPlatform_State::Expanding:
		Platform_Expanding.Draw_State(hdc, X_Pos);
		break;

	case EPlatform_State::Laser:
		Platform_Laser.Draw_State(hdc, X_Pos, Curr_Rect);
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform::Is_Finished()
{
	return false;  // Заглушка. Этот метод не используется
}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform::Hit_By(AFalling_Letter *falling_letter) const
{
	RECT intersection_rect, letter_cell;

	falling_letter->Get_Cell(letter_cell);

	if (IntersectRect(&intersection_rect, &letter_cell, &Curr_Rect) )
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
double AsPlatform::Get_Middle_Pos() const
{
	return (X_Pos + (double)AsConfig::Platform_Normal_Width / 2.0);
}
//------------------------------------------------------------------------------------------------------------
double AsPlatform::Get_X_Offset() const
{
	return (X_Pos - Prev_X_Pos);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::On_Space_Key(bool key_down)
{
	if (Platform_State == EPlatform_State::Laser)
		Platform_Laser.Fire(key_down);

	if (! key_down)
		return;

	if (Has_State(EPlatform_Substate_Regular::Ready) )
	{
		Ball_Set->Release_From_Platform(Get_Middle_Pos() );
		Set_State(EPlatform_Substate_Regular::Normal);
	}
	else if (Platform_State == EPlatform_State::Glue && Platform_State.Glue == EPlatform_Transformation::Active)
		Ball_Set->Release_From_Glue_Platform();
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Init(AsBall_Set *ball_set, AsLaser_Beam_Set *laser_beams_set)
{
	Platform_Expanding.Init(Inner_Color, Circle_Color, Highlight_Color);
	Platform_Laser.Init(Inner_Color, Circle_Color, laser_beams_set);

	Ball_Set = ball_set;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Redraw()
{
	double left_rect_x;

	if (Current_Redraw_Timer_Tick != AsConfig::Current_Timer_Tick)
	{
		Prev_Rect = Curr_Rect;
		Current_Redraw_Timer_Tick = AsConfig::Current_Timer_Tick;
	}

	if (Platform_State == EPlatform_State::Meltdown)
		Prev_Rect.bottom = (AsConfig::Play_Area_Max_Y_Offset + 1) * AsConfig::Global_Scale;

	left_rect_x = X_Pos * AsConfig::D_Global_Scale;

	Curr_Rect.left = (int)(left_rect_x);
	Curr_Rect.top = AsConfig::Platform_Y_Pos * AsConfig::Global_Scale;
	Curr_Rect.right = (int)(left_rect_x + Get_Current_Width() * AsConfig::D_Global_Scale);
	Curr_Rect.bottom = Curr_Rect.top + AsConfig::Platform_Height * AsConfig::Global_Scale;

	AsTools::Invalidate_Rect(Prev_Rect);
	AsTools::Invalidate_Rect(Curr_Rect);
}
//------------------------------------------------------------------------------------------------------------
EPlatform_State AsPlatform::Get_State() const
{
	return Platform_State;
}
//------------------------------------------------------------------------------------------------------------
EPlatform_Moving_State AsPlatform::Get_Moving_State() const
{
	return Platform_State.Moving;
}
//------------------------------------------------------------------------------------------------------------
double AsPlatform::Get_Moving_Direction() const
{
	if (Platform_State.Moving == EPlatform_Moving_State::Moving_Left)
		return M_PI;
	else if (Platform_State.Moving == EPlatform_Moving_State::Moving_Right)
		return 0.0;
	else
		return 0.0;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Set_State(EPlatform_State new_state)
{
	if (Platform_State == new_state)
		return;

	switch (new_state)
	{
	case EPlatform_State::Regular:
		AsTools::Throw();  // для установки платформы в регулярное состояние неявно вызывается Set_State(EPlatform_Substate_Regular new_regular_state)
		break;


	case EPlatform_State::Rolling:
		Platform_State.Rolling = EPlatform_Substate_Rolling::Roll_In;
		X_Pos = (double)AsConfig::Level_Max_X_Offset + 3.0;
		Rolling_Step = Max_Rolling_Step;
		break;


	case EPlatform_State::Meltdown:
		if (Platform_State != EPlatform_State::Regular)
			return Platform_State.Set_Next_State(new_state);

		Platform_State.Moving = EPlatform_Moving_State::Stop;
		Left_Key_Down = false;
		Right_Key_Down = false;
		Speed = 0.0;
		Platform_State.Meltdown = EPlatform_Substate_Meltdown::Init;
		break;


	case EPlatform_State::Glue:
		if (Set_Transformation_State(new_state, Platform_State.Glue) )
			return;
		break;


	case EPlatform_State::Expanding:
		if (Set_Transformation_State(new_state, Platform_State.Expanding) )
			return;
		else
			Platform_Expanding.Reset();
		break;


	case EPlatform_State::Laser:
		if (Set_Transformation_State(new_state, Platform_State.Laser) )
			return;
		break;
	}

	Platform_State = new_state;
}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform::Set_Transformation_State(EPlatform_State new_state, EPlatform_Transformation &transformation_state)
{
	if (Platform_State != EPlatform_State::Regular)
	{
		Platform_State.Set_Next_State(new_state);
		return true;
	}

	if (transformation_state == EPlatform_Transformation::Finalize)
		return true;
	else
	{
		transformation_state = EPlatform_Transformation::Init;
		return false;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Set_State(EPlatform_Substate_Regular new_regular_substate)
{
	EPlatform_State next_state;

	next_state = Platform_State.Set_State(new_regular_substate);

	if (next_state != EPlatform_State::Unknown)
		Set_State(next_state);
}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform::Has_State(EPlatform_Substate_Regular regular_state) const
{
	if (Platform_State != EPlatform_State::Regular)
		return false;

	return Platform_State.Regular == regular_state;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Move(bool to_left, bool key_down)
{
	if ( ! (Has_State(EPlatform_Substate_Regular::Normal) || Platform_State == EPlatform_State::Glue ||
		Platform_State == EPlatform_State::Expanding || Platform_State == EPlatform_State::Laser) )
		return;

	if (to_left)
		Left_Key_Down = key_down;
	else
		Right_Key_Down = key_down;

	if (Left_Key_Down && Right_Key_Down)
		return;

	if (! Left_Key_Down && ! Right_Key_Down)
	{
		Speed = 0.0;
		Platform_State.Moving = EPlatform_Moving_State::Stopping;
		return;
	}

	if (Left_Key_Down)
	{
		Platform_State.Moving = EPlatform_Moving_State::Moving_Left;
		Speed = -Platform_Normal_Speed;
	}

	if (Right_Key_Down)
	{
		Platform_State.Moving = EPlatform_Moving_State::Moving_Right;
		Speed = Platform_Normal_Speed;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Rolling_State(HDC hdc)
{
	switch (Platform_State.Rolling)
	{
	case EPlatform_Substate_Rolling::Roll_In:
		Draw_Roll_In_Substate(hdc);
		break;

	case EPlatform_Substate_Rolling::Expand_Roll_In:
		Draw_Normal_State(hdc);
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Roll_In_Substate(HDC hdc)
{
	int x = Curr_Rect.left;
	int y = Curr_Rect.top;
	int roller_size = AsConfig::Platform_Circle_Diameter * AsConfig::Global_Scale;
	double alpha;
	XFORM xform{}, old_xform;

	// 1. Шарик
	Circle_Color.Select(hdc);
	Ellipse(hdc, x, y, x + roller_size - 1, y + roller_size - 1);

	// 2. Разделительная линия
	alpha = -2.0 * M_PI / (double)Max_Rolling_Step * (double)Rolling_Step;

	xform.eM11 = (float)cos(alpha);
	xform.eM12 = (float)sin(alpha);
	xform.eM21 = (float)-sin(alpha);
	xform.eM22 = (float)cos(alpha);
	xform.eDx = (float)(x + roller_size / 2);
	xform.eDy = (float)(y + roller_size / 2);
	GetWorldTransform(hdc, &old_xform);
	SetWorldTransform(hdc, &xform);

	AsConfig::BG_Color.Select(hdc);

	Rectangle(hdc, -AsConfig::Global_Scale / 2, -roller_size / 2, AsConfig::Global_Scale / 2, roller_size / 2);

	SetWorldTransform(hdc, &old_xform);

	// 3. Блик
	Platform_Expanding.Draw_Circle_Highlight(hdc, x, y);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Normal_State(HDC hdc)
{
	int x = Curr_Rect.left;
	const int y = Curr_Rect.top;
	int inner_x_offset = 4 * AsConfig::Global_Scale;
	int inner_y_offset = 1 * AsConfig::Global_Scale;
	RECT inner_rect{};

	// 1. Шарики
	Circle_Color.Select(hdc);

	Ellipse(hdc, x, y, x + AsConfig::Platform_Circle_Diameter * AsConfig::Global_Scale - 1, y + AsConfig::Platform_Circle_Diameter * AsConfig::Global_Scale - 1);
	Ellipse(hdc, x + Inner_Width * AsConfig::Global_Scale, y, x + (Inner_Width + AsConfig::Platform_Circle_Diameter) * AsConfig::Global_Scale - 1, y + AsConfig::Platform_Circle_Diameter * AsConfig::Global_Scale - 1);

	// 2. Блик
	Platform_Expanding.Draw_Circle_Highlight(hdc, x, y);

	// 3. Внутренняя часть
	inner_rect.left = x + inner_x_offset;
	inner_rect.top = y + inner_y_offset;
	inner_rect.right = inner_rect.left + Inner_Width * AsConfig::Global_Scale;
	inner_rect.bottom = y - inner_y_offset + AsConfig::Platform_Circle_Diameter * AsConfig::Global_Scale;

	Inner_Color.Select(hdc);
	AsTools::Round_Rect(hdc, inner_rect, 3);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Meltdown_State(HDC hdc, RECT &paint_area)
{//!!! Нет реализации состояния Meltdown
	
	Clear(hdc, paint_area);

	Platform_State.Meltdown = EPlatform_Substate_Meltdown::Unknown;

	Set_State(EPlatform_Substate_Regular::Missing);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Act_Rolling_State()
{
	switch (Platform_State.Rolling)
	{
	case EPlatform_Substate_Rolling::Roll_In:
		++Rolling_Step;

		if (Rolling_Step >= Max_Rolling_Step)
			Rolling_Step -= Max_Rolling_Step;

		X_Pos -= Rolling_Platform_Speed;

		if (X_Pos <= Roll_In_Platform_End_X_Pos)
		{
			X_Pos += Rolling_Platform_Speed;
			Inner_Width = 1;
			Platform_State.Rolling = EPlatform_Substate_Rolling::Expand_Roll_In;
		}
		break;

	case EPlatform_Substate_Rolling::Expand_Roll_In:
		--X_Pos;
		Inner_Width += 2;

		if (Inner_Width > Normal_Inner_Width)
		{
			Inner_Width = Normal_Inner_Width;
			Platform_State.Rolling = EPlatform_Substate_Rolling::Unknown;
			Set_State(EPlatform_Substate_Regular::Ready);
		}
		break;
	}


	Redraw();
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Act_Meltdown_State()
{
	switch (Platform_State.Meltdown)
	{
	case EPlatform_Substate_Meltdown::Init:
		Platform_State.Meltdown = EPlatform_Substate_Meltdown::Active;
		Redraw();
		break;

	case EPlatform_Substate_Meltdown::Active:
		break;  //!!! Нет реализации состояния Meltdown
	}
}
//------------------------------------------------------------------------------------------------------------
double AsPlatform::Get_Current_Width() const
{
	if (Platform_State == EPlatform_State::Rolling && Platform_State.Rolling == EPlatform_Substate_Rolling::Roll_In)
		return (double)AsConfig::Platform_Circle_Diameter;
	else if (Platform_State == EPlatform_State::Expanding)
		return Platform_Expanding.Curr_Expanding_Width;
	else
		return (double)AsConfig::Platform_Normal_Width;
}
//------------------------------------------------------------------------------------------------------------
