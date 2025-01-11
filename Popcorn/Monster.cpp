#include "Monster.h"


// AMonster
//------------------------------------------------------------------------------------------------------------
AMonster::~AMonster()
{
}
//------------------------------------------------------------------------------------------------------------
AMonster::AMonster()
	: Monster_State(EMonster_State::Missing), Need_To_Freeze(false),
	X_Pos(0.0), Y_Pos(0.0), Emiting_Timer_Tick(0), Direction_Change_Timer_Tick(0),
	Direction(0.0), Prev_Speed(0.0), Monster_Rect{}, Prev_Monster_Rect{}, Explosive_Balls(Max_Explosive_Balls_Count)
{
}
//------------------------------------------------------------------------------------------------------------
bool AMonster::Check_Hit(double next_x_pos, double next_y_pos, ABall_Object *ball)
{
	double center_x_pos, center_y_pos;
	double circle_radius;

	if (! (Monster_State == EMonster_State::Alive || Monster_State == EMonster_State::Emitting) )
		return false;

	circle_radius = (double)Monster_Size / 2.0;
	center_x_pos = X_Pos + circle_radius;
	center_y_pos = Y_Pos + circle_radius;

	if(! AsTools::Reflect_From_Circle(next_x_pos, next_y_pos,  circle_radius, center_x_pos, center_y_pos, ball) )
		return false;

	Destroy();

	return true;
}
//------------------------------------------------------------------------------------------------------------
bool AMonster::Check_Hit(double next_x_pos, double next_y_pos)
{
	if (! (Monster_State == EMonster_State::Alive || Monster_State == EMonster_State::Emitting) )
		return false;

	if (next_y_pos >= Y_Pos && next_y_pos <= Y_Pos + Monster_Size &&
		next_x_pos >= X_Pos && next_x_pos <= X_Pos + Monster_Size)
	{
		Destroy();
		return true;
	}
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
bool AMonster::Check_Hit(RECT &rect)
{
	RECT intersection_rect;

	if (! (Monster_State == EMonster_State::Alive || Monster_State == EMonster_State::Emitting) )
		return false;

	if (IntersectRect(&intersection_rect, &rect, &Monster_Rect) )
	{
		Destroy();
		return true;
	}
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Begin_Movement()
{
	if (Monster_State == EMonster_State::Missing)
		return;

	Prev_Monster_Rect = Monster_Rect;
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Finish_Movement()
{
	if (Monster_State == EMonster_State::Missing)
		return;

	Redraw_Monster();
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Advance(double max_speed)
{
	int i;
	bool got_new_direction = false;
	double origin_direction;
	double next_step;
	double next_x_pos, next_y_pos;
	RECT monster_rect;

	if (! (Monster_State == EMonster_State::Alive || Monster_State == EMonster_State::Emitting) || Speed == 0.0)
		return;

	if (Monster_State == EMonster_State::Emitting)
	{
		next_step = Speed / max_speed * AsConfig::Moving_Step_Size;

		next_x_pos = X_Pos + next_step * cos(Direction);
		next_y_pos = Y_Pos - next_step * sin(Direction);

		X_Pos = next_x_pos;
		Y_Pos = next_y_pos;

		return;
	}

	origin_direction = Direction;

	for (i = 0; i < 16; i++)
	{
		next_step = Speed / max_speed * AsConfig::Moving_Step_Size;

		next_x_pos = X_Pos + next_step * cos(Direction);
		next_y_pos = Y_Pos - next_step * sin(Direction);

		Get_Monster_Rect(next_x_pos, next_y_pos, monster_rect);

		if ( ! AsLevel::Has_Brick_At(monster_rect) )
		{
			got_new_direction = true;
			break;
		}
		else
			Direction += M_PI / 8.0;
	}

	if ( ! got_new_direction)
		Direction = origin_direction - M_PI;

	if (next_x_pos < AsConfig::Level_X_Offset)
		next_x_pos = AsConfig::Level_X_Offset;
	else if (next_x_pos + Monster_Size > AsConfig::Level_Max_X_Offset)
		next_x_pos = AsConfig::Level_Max_X_Offset - Monster_Size;

	if (next_y_pos < AsConfig::Level_Y_Offset - 1)
		next_y_pos = AsConfig::Level_Y_Offset - 1;
	else if (next_y_pos + Monster_Size > AsConfig::Floor_Y_Pos)
		next_y_pos = AsConfig::Floor_Y_Pos - Monster_Size;

	X_Pos = next_x_pos;
	Y_Pos = next_y_pos;
}
//------------------------------------------------------------------------------------------------------------
double AMonster::Get_Speed()
{
	return Speed;
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Act()
{
	switch (Monster_State)
	{
	case EMonster_State::Missing:
		return;

	case EMonster_State::Emitting:
		Act_Alive();

		if (AsConfig::Current_Timer_Tick >= Emiting_Timer_Tick)
			Monster_State = EMonster_State::Alive;
		break;

	case EMonster_State::Alive:
		if (Need_To_Freeze)
		{
			Prev_Speed = Speed;
			Speed = 0.0;
		}
		Act_Alive();
		Change_Direction();
		break;

	case EMonster_State::Destroing:
		Act_Destroing();
		break;

	default:
		AsTools::Throw();
	}
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Clear(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if (Monster_State == EMonster_State::Missing)
		return;

	if ( ! IntersectRect(&intersection_rect, &paint_area, &Prev_Monster_Rect) )
		return;

	AsTools::Ellipse(hdc, Prev_Monster_Rect, AsConfig::BG_Color);
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Draw(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if ( ! IntersectRect(&intersection_rect, &paint_area, &Monster_Rect) )
		return;

	switch (Monster_State)
	{
	case EMonster_State::Missing:
		break;

	case EMonster_State::Emitting:
	case EMonster_State::Alive:
		Draw_Alive(hdc);
		break;

	case EMonster_State::Destroing:
		Draw_Destroing(hdc, paint_area);
		break;

	default:
		AsTools::Throw();
	}
}
//------------------------------------------------------------------------------------------------------------
bool AMonster::Is_Finished()
{
	if (Monster_State == EMonster_State::Missing)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Let_Out(int gate_x_pos, int gate_y_pos, bool left_gate)
{
	Monster_State = EMonster_State::Emitting;

	X_Pos = gate_x_pos;
	Y_Pos = gate_y_pos + 1;
	Speed = (double)(AsTools::Rand(5) + 1) / 10.0;

	if (left_gate)
		Direction = 0.0;
	else
	{
		X_Pos -= Monster_Size - AGate::Width;
		Direction = M_PI;
	}

	Emiting_Timer_Tick = AsConfig::Current_Timer_Tick + (int)( (double)AGate::Width / (double)Speed);

	On_Activation();
	Redraw_Monster();
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Destroy()
{
	bool is_red;
	int monster_half_size;
	int monster_x_center, monster_Y_center;
	int x_offset, y_offset;
	int size;
	int delay;
	
	if (! (Monster_State == EMonster_State::Alive || Monster_State == EMonster_State::Emitting) )
		return;

	monster_half_size = Monster_Size / 2;
	monster_x_center = (int)X_Pos + monster_half_size;
	monster_Y_center = (int)Y_Pos + monster_half_size;

	for (auto &ball : Explosive_Balls)
	{
		is_red = AsTools::Rand(2);
		delay = AsTools::Rand(20);
		x_offset = AsTools::Rand(monster_half_size) - monster_half_size / 2;
		y_offset = AsTools::Rand(monster_half_size) - monster_half_size / 2;

		size = (monster_half_size - (int)sqrt(x_offset * x_offset + y_offset * y_offset) ) * 2;

		ball.Explode(monster_x_center + x_offset, monster_Y_center + y_offset, size, delay, is_red);
	}
	
	Monster_State = EMonster_State::Destroing;

	AsInfo_Panel::Update_Score(EScore_Event_Type::Hit_Monster);
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Set_Freeze_State(bool is_freeze)
{
	Need_To_Freeze = is_freeze;

	if (! is_freeze)
		Speed = Prev_Speed;
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Act_Destroing()
{
	bool explode_finished = true;

	for (auto &ball : Explosive_Balls)
	{
		ball.Act();

		explode_finished &= ball.Is_Finished();
	}

	if (explode_finished)
		Monster_State = EMonster_State::Missing;
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Draw_Destroing(HDC hdc, RECT &paint_area)
{
	for (auto &ball : Explosive_Balls)
		ball.Draw(hdc, paint_area);
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Get_Monster_Rect(double x_pos, double y_pos, RECT &rect)
{
	rect.left = (int)(x_pos * AsConfig::D_Global_Scale);
	rect.top = (int)(y_pos * AsConfig::D_Global_Scale);
	rect.right = rect.left + Monster_Size * AsConfig::Global_Scale;
	rect.bottom = rect.top + Monster_Size * AsConfig::Global_Scale;
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Redraw_Monster()
{
	Get_Monster_Rect(X_Pos, Y_Pos, Monster_Rect);

	AsTools::Invalidate_Rect(Prev_Monster_Rect);
	AsTools::Invalidate_Rect(Monster_Rect);
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Change_Direction()
{
	if (AsConfig::Current_Timer_Tick < Direction_Change_Timer_Tick)
		return;
	
	Direction_Change_Timer_Tick += AsTools::Rand(AsConfig::FPS);

	Direction += (double)(AsTools::Rand(90) - 45) * M_PI / 180.0;
}
//------------------------------------------------------------------------------------------------------------




// AMonster_Eye
const double AMonster_Eye::Max_Cornea_Height = 7.0;
const double AMonster_Eye::Blink_Timeouts[] = { 0.7, 0.1, 1.0, 0.2, 0.2, 1.5, 0.5 };
const EEye_State AMonster_Eye::Blink_States[] =
{
	EEye_State::Closed, EEye_State::Opening, EEye_State::Staring, EEye_State::Closing,
	EEye_State::Opening, EEye_State::Staring, EEye_State::Closing
};
//------------------------------------------------------------------------------------------------------------
AMonster_Eye::~AMonster_Eye()
{
}
//------------------------------------------------------------------------------------------------------------
AMonster_Eye::AMonster_Eye()
: Eye_State(EEye_State::Closed), Start_Blink_Timer_Tick(0), Total_Animation_Timeout(0), Cornea_Height(0.0),
  Blink_Ticks(Blinks_Stages_Count)
{
}
//------------------------------------------------------------------------------------------------------------
void AMonster_Eye::Act_Alive()
{
	int i;
	int current_tick_offset, prev_tick;
	double ratio;

	current_tick_offset = (AsConfig::Current_Timer_Tick - Start_Blink_Timer_Tick) % Total_Animation_Timeout;

	for (i = 0; i < Blinks_Stages_Count; i++)
	{
		if (current_tick_offset < Blink_Ticks[i])
		{
			Eye_State = Blink_States[i];
			break;
		}
	}

	if (i == 0)
		prev_tick = 0;
	else
		prev_tick = Blink_Ticks[i - 1];

	ratio = (double)(current_tick_offset - prev_tick) / (double)(Blink_Ticks[i] - prev_tick);

	switch (Eye_State)
	{
	case EEye_State::Closed:
		Cornea_Height = 0.0;
		break;

	case EEye_State::Opening:
		Cornea_Height = Max_Cornea_Height * ratio;
		break;

	case EEye_State::Staring:
		Cornea_Height = Max_Cornea_Height;
		break;

	case EEye_State::Closing:
		Cornea_Height = Max_Cornea_Height - Max_Cornea_Height * ratio;
		break;

	default:
		AsTools::Throw();
	}

	AsTools::Invalidate_Rect(Monster_Rect);
}
//------------------------------------------------------------------------------------------------------------
void AMonster_Eye::Draw_Alive(HDC hdc)
{
	const int scale = AsConfig::Global_Scale;
	const double d_scale = AsConfig::D_Global_Scale;
	int monster_y_center = (int)( (Y_Pos + (double)Monster_Size / 2.0) * d_scale);
	int cornea_x_offset = 1;
	int iris_x_offset = 4;
	int iris_half_height = 3;
	int pupil_x_offset = 7;
	int pupil_half_height = 1;
	RECT cornea_rect{}, iris_rect{}, pupil_rect{};
	HRGN cornea_rgn;

	// 1. Монстр
	AsTools::Ellipse(hdc, Monster_Rect, AsConfig::Red_Color);

	// 2. Глаз
	// 2.1 Роговица
	cornea_rect.left = Monster_Rect.left + cornea_x_offset * scale;
	cornea_rect.top = monster_y_center - (int)(Cornea_Height / 2.0 * d_scale) - scale;
	cornea_rect.right = Monster_Rect.right - cornea_x_offset * scale;
	cornea_rect.bottom = cornea_rect.top + (int)(Cornea_Height * d_scale);

	AsTools::Ellipse(hdc, cornea_rect, AsConfig::Cornea_Color);

	// 2.2 Радужка
	cornea_rgn = CreateRectRgnIndirect(&cornea_rect);
	SelectClipRgn(hdc, cornea_rgn);

	iris_rect.left = Monster_Rect.left + iris_x_offset * scale;
	iris_rect.top = monster_y_center - iris_half_height * scale - scale;
	iris_rect.right = Monster_Rect.right - iris_x_offset * scale;
	iris_rect.bottom = monster_y_center + iris_half_height * scale;

	AsTools::Ellipse(hdc, iris_rect, AsConfig::Iris_Color);

	// 2.3 Хрусталик
	pupil_rect.left = Monster_Rect.left + pupil_x_offset * scale;
	pupil_rect.top = monster_y_center - pupil_half_height * scale - scale;
	pupil_rect.right = Monster_Rect.right - pupil_x_offset * scale;
	pupil_rect.bottom = monster_y_center + pupil_half_height * scale;

	AsTools::Rect(hdc, pupil_rect, AsConfig::BG_Color);

	SelectClipRgn(hdc, 0);
	DeleteObject(cornea_rgn);

	// 2.4 Обводка
	AsConfig::Cornea_Color.Select_Pen(hdc);
	Arc(hdc, cornea_rect.left, cornea_rect.top, cornea_rect.right, cornea_rect.bottom, 0, 0, 0, 0);
}
//------------------------------------------------------------------------------------------------------------
void AMonster_Eye::On_Activation()
{
	int i;
	int tick_offset;
	double curr_tick = 0;

	Start_Blink_Timer_Tick = AsConfig::Current_Timer_Tick;

	for (i = 0; i < Blinks_Stages_Count; i++)
	{
		curr_tick += Blink_Timeouts[i];
		tick_offset = (int)( (double)AsConfig::FPS * curr_tick);
		Blink_Ticks[i] = tick_offset;
	}

	Total_Animation_Timeout = tick_offset;
}
//------------------------------------------------------------------------------------------------------------




// AMonster_Comet
//------------------------------------------------------------------------------------------------------------
AMonster_Comet::~AMonster_Comet()
{
}
//------------------------------------------------------------------------------------------------------------
AMonster_Comet::AMonster_Comet()
	: Alive_Timer_Tick(0), Ticks_Per_Rotation(0), Current_Angle(0.0)
{
}
//------------------------------------------------------------------------------------------------------------
void AMonster_Comet::Clear(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if (Monster_State == EMonster_State::Missing)
		return;

	if ( ! IntersectRect(&intersection_rect, &paint_area, &Prev_Monster_Rect) )
		return;

	AsTools::Rect(hdc, Prev_Monster_Rect, AsConfig::BG_Color);
}
//------------------------------------------------------------------------------------------------------------
void AMonster_Comet::Act_Alive()
{
	int tick_offset;
	double ratio;

	tick_offset = (AsConfig::Current_Timer_Tick - Alive_Timer_Tick) % Ticks_Per_Rotation;

	ratio = (double)tick_offset / (double)Ticks_Per_Rotation;

	Current_Angle = ratio * 2.0 * M_PI;
}
//------------------------------------------------------------------------------------------------------------
void AMonster_Comet::Draw_Alive(HDC hdc)
{
	int i;
	double alpha;
	const int scale = AsConfig::Global_Scale;
	const double d_scale = AsConfig::D_Global_Scale;
	double monster_radius = (double)(Monster_Size * d_scale / 2.0 - 1.0);
	int ball_size = 4 * scale - scale / 2;
	XFORM xform{}, old_xform;
	RECT rect{};

	GetWorldTransform(hdc, &old_xform);

	alpha = Current_Angle;

	for (i = 0; i < 2; i++)
	{
		xform.eM11 = (float)cos(alpha);
		xform.eM12 = (float)sin(alpha);
		xform.eM21 = (float)-sin(alpha);
		xform.eM22 = (float)cos(alpha);
		xform.eDx = (float)(X_Pos * d_scale + monster_radius);
		xform.eDy = (float)(Y_Pos * d_scale + monster_radius);
	
		SetWorldTransform(hdc, &xform);
		alpha += M_PI;

		rect.left = (int)-monster_radius;
		rect.top = -ball_size / 2;
		rect.right = rect.left + ball_size;
		rect.bottom = rect.top + ball_size;

		AsTools::Ellipse(hdc, rect, AsConfig::White_Color);

		AsConfig::Monster_Comet_Tail.Select_Pen(hdc);

		rect.left = (int)-monster_radius + 2 * scale;
		rect.top = (int)-monster_radius + 2 * scale;
		rect.right = (int)monster_radius - 2 * scale;
		rect.bottom = (int)monster_radius - 2 * scale;

		Arc(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1, 0, (int)-monster_radius, (int)-monster_radius, - 4 * scale);
	
		rect.left += 1 * scale;

		Arc(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1, 0, (int)-monster_radius, (int)-monster_radius, - 4 * scale);
	}

	SetWorldTransform(hdc, &old_xform);
}
//------------------------------------------------------------------------------------------------------------
void AMonster_Comet::On_Activation()
{
	int ticks_delta;
	Alive_Timer_Tick = AsConfig::Current_Timer_Tick;

	ticks_delta = Max_Ticks_Per_Rotation - Min_Ticks_Per_Rotation;

	Ticks_Per_Rotation = AsTools::Rand(ticks_delta) + Min_Ticks_Per_Rotation;
}
//------------------------------------------------------------------------------------------------------------
