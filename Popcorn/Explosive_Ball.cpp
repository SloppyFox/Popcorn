#include "Explosive_Ball.h"

// AExplosive_Ball
AColor AExplosive_Ball::Fading_Red_Colors[Fading_Steps_Count];
AColor AExplosive_Ball::Fading_Blue_Colors[Fading_Steps_Count];
//------------------------------------------------------------------------------------------------------------
AExplosive_Ball::AExplosive_Ball()
	: Explosive_Ball_State(EExplosive_Ball_State::Idle), Is_Red(false), X_Pos(0.0), Y_Pos(0.0),
	Start_Explode_Tick(0), Start_Fading_Tick(0), End_Explode_Tick(0),
	Size(0), Max_Size(0), Curr_Rect{}, Curr_Ball_Color(0)
{
}
//------------------------------------------------------------------------------------------------------------
void AExplosive_Ball::Act()
{
	switch (Explosive_Ball_State)
	{
	case EExplosive_Ball_State::Idle:
		return;

	case EExplosive_Ball_State::Waiting:
		if (AsConfig::Current_Timer_Tick >= Start_Explode_Tick)
		{
			Explosive_Ball_State = EExplosive_Ball_State::Expanding;
		}
		break;

	case EExplosive_Ball_State::Expanding:
		if (AsConfig::Current_Timer_Tick >= Start_Fading_Tick)
			Explosive_Ball_State = EExplosive_Ball_State::Fading;
		else
			Act_Expanding_State();
		break;

	case EExplosive_Ball_State::Fading:
		if (AsConfig::Current_Timer_Tick >= End_Explode_Tick)
			Explosive_Ball_State = EExplosive_Ball_State::Idle;
		else
			Act_Fading_State();
		break;

	default:
		AsTools::Throw();
	}
}
//------------------------------------------------------------------------------------------------------------
void AExplosive_Ball::Clear(HDC hdc, RECT &paint_area)
{
	//!!!
}
//------------------------------------------------------------------------------------------------------------
void AExplosive_Ball::Draw(HDC hdc, RECT &paint_area)
{
	if (Explosive_Ball_State == EExplosive_Ball_State::Idle)
		return;

	if (Curr_Ball_Color != 0)
		AsTools::Ellipse(hdc, Curr_Rect, *Curr_Ball_Color);
	else
		AsTools::Throw();
}
//------------------------------------------------------------------------------------------------------------
bool AExplosive_Ball::Is_Finished()
{
	if (Explosive_Ball_State == EExplosive_Ball_State::Idle)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
void AExplosive_Ball::Explode(double x_pos, double y_pos, double max_size, int explode_delay_ticks, bool is_red)
{
	if (explode_delay_ticks > 0.0)
		Explosive_Ball_State = EExplosive_Ball_State::Waiting;
	else
		Explosive_Ball_State = EExplosive_Ball_State::Expanding;

	X_Pos = x_pos;
	Y_Pos = y_pos;
	Max_Size = max_size;
	Size = 0.0;
	Is_Red = is_red;

	if (is_red)
		Curr_Ball_Color = &Fading_Red_Colors[0];
	else
		Curr_Ball_Color = &Fading_Blue_Colors[0];

	Start_Explode_Tick = explode_delay_ticks + AsConfig::Current_Timer_Tick;
	Start_Fading_Tick = Start_Explode_Tick + Expanding_Timeout;
	End_Explode_Tick = Start_Fading_Tick + Fading_Timeout;
}
//------------------------------------------------------------------------------------------------------------
void AExplosive_Ball::Setup_Colors()
{
	int i;

	for (i = 0; i < Fading_Steps_Count; i++)
	{
		AsTools::Get_Fading_Color(AsConfig::Red_Color, i, Fading_Red_Colors[i], Fading_Steps_Count);
		AsTools::Get_Fading_Color(AsConfig::Blue_Color, i, Fading_Blue_Colors[i], Fading_Steps_Count);
	}
}
//------------------------------------------------------------------------------------------------------------
void AExplosive_Ball::Act_Expanding_State()
{
	double ratio;

	ratio = (double)(AsConfig::Current_Timer_Tick - Start_Explode_Tick) / (double)Expanding_Timeout;

	Size = Max_Size * ratio;

	Update_Rect();
	AsTools::Invalidate_Rect(Curr_Rect);
}
//------------------------------------------------------------------------------------------------------------
void AExplosive_Ball::Act_Fading_State()
{
	int curr_color_index;
	double ratio;

	ratio = (double)(AsConfig::Current_Timer_Tick - Start_Fading_Tick) / (double)Expanding_Timeout;

	curr_color_index = (int)round( (double)Fading_Steps_Count * ratio);

	if (curr_color_index < 0)
		curr_color_index = 0;
	if (curr_color_index > Fading_Steps_Count - 1)
		curr_color_index = Fading_Steps_Count - 1;

	if (Is_Red)
		Curr_Ball_Color = &Fading_Red_Colors[curr_color_index];
	else
		Curr_Ball_Color = &Fading_Blue_Colors[curr_color_index];

	AsTools::Invalidate_Rect(Curr_Rect);
}
//------------------------------------------------------------------------------------------------------------
void AExplosive_Ball::Update_Rect()
{
	Curr_Rect.left = (int)( (X_Pos - Size / 2.0) * AsConfig::D_Global_Scale);
	Curr_Rect.top = (int)( (Y_Pos - Size / 2.0) * AsConfig::D_Global_Scale);
	Curr_Rect.right = (int)( (X_Pos + Size / 2.0) * AsConfig::D_Global_Scale);
	Curr_Rect.bottom = (int)( (Y_Pos + Size / 2.0) * AsConfig::D_Global_Scale);
}
//------------------------------------------------------------------------------------------------------------
