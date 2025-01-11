#include "Ball_Set.h"

// AsBall_Set
//-----------------------------------------------------------------------------------------------------------
AsBall_Set::AsBall_Set()
	: Balls(AsConfig::Max_Balls_Count)
{
}
//-----------------------------------------------------------------------------------------------------------
void AsBall_Set::Act()
{
	for (auto &ball : Balls)
		if (ball.Get_State() == EBall_State::On_Glue_Platform)
			if (AsConfig::Current_Timer_Tick == ball.Release_Timer_Tick)
				ball.Set_State(EBall_State::Normal);
}
//-----------------------------------------------------------------------------------------------------------
void AsBall_Set::Set_On_Platform(double platform_x_pos)
{
	int i;

	Balls[0].Set_State(EBall_State::On_Platform, platform_x_pos, AsConfig::Start_Ball_Y_Pos);

	for (i = 1; i < (int)Balls.size(); i++)
		Balls[i].Set_State(EBall_State::Disabled);
}
//-----------------------------------------------------------------------------------------------------------
void AsBall_Set::Release_From_Platform(double platform_x_pos)
{
	for (auto &ball : Balls)
		if (ball.Get_State() == EBall_State::On_Platform)
			ball.Set_State(EBall_State::Normal);
}
//-----------------------------------------------------------------------------------------------------------
bool AsBall_Set::Release_From_Glue_Platform()
{
	for (auto &ball : Balls)
		if (ball.Get_State() == EBall_State::On_Glue_Platform)
		{
			ball.Set_State(EBall_State::Normal);
			return true;
		}

	return false;
}
//-----------------------------------------------------------------------------------------------------------
bool AsBall_Set::Are_All_Balls_Lost()
{
	int active_balls_count = 0;
	int lost_balls_count = 0;

	for (auto &ball : Balls)
	{
		if (ball.Get_State() == EBall_State::Disabled)
			continue;

		++active_balls_count;

		if (ball.Get_State() == EBall_State::Lost)
			++lost_balls_count;
	}

	if (active_balls_count == lost_balls_count)
		return true;
	else
		return false;
}
//-----------------------------------------------------------------------------------------------------------
void AsBall_Set::Set_For_Test()
{
	Balls[0].Set_For_Test();
}
//-----------------------------------------------------------------------------------------------------------
bool AsBall_Set::Is_Test_Finished()
{
	return Balls[0].Is_Test_Finished();
}
//-----------------------------------------------------------------------------------------------------------
void AsBall_Set::Triple_Balls()
{// растраивает самый дальний мячик от платформы

	double curr_ball_x, curr_ball_y;
	double furrest_ball_x, furrest_ball_y;
	ABall *furrest_ball = 0;
	ABall *left_candidate = 0, *right_candidate = 0;

	// 1. вычисляем самый дальний мячик от платформы
	for (auto &ball : Balls)
	{
		if (ball.Get_State() != EBall_State::Normal)
			continue;

		if (furrest_ball == 0)
			furrest_ball = &ball;
		else
		{
			ball.Get_Center(curr_ball_x, curr_ball_y);
			furrest_ball->Get_Center(furrest_ball_x, furrest_ball_y);

			if (curr_ball_y < furrest_ball_y)
				furrest_ball = &ball;
		}
	}

	// 2. если нашли мячик в состоянии EBall_State::Normal, ищем свободных кандидатов на размножение
	if (furrest_ball == 0)
		return;

	for (auto &ball : Balls)
	{
		switch (ball.Get_State() )
		{
		case EBall_State::Disabled:
		case EBall_State::Lost:
			if (left_candidate == 0)
				left_candidate = &ball;
			else if (right_candidate == 0)
				right_candidate = &ball;
		}
	}

	// 3. задаем параметры для кандидатов
	if (left_candidate != 0)
	{
		*left_candidate = *furrest_ball;
		left_candidate->Set_Direction(left_candidate->Get_Direction() + M_PI / AsConfig::Min_Ball_Angle);
	}
	if (right_candidate != 0)
	{
		*right_candidate = *furrest_ball;
		right_candidate->Set_Direction(right_candidate->Get_Direction() - M_PI / AsConfig::Min_Ball_Angle);
	}
}
//-----------------------------------------------------------------------------------------------------------
void AsBall_Set::Inverse_Direction_Balls()
{
	for (auto &ball : Balls)
		if (ball.Get_State() == EBall_State::Normal)
			ball.Set_Direction(ball.Get_Direction() + M_PI);
}
//-----------------------------------------------------------------------------------------------------------
void AsBall_Set::Accelerate()
{
	for (auto &ball : Balls)
		if (ball.Get_State() == EBall_State::Normal)
			ball.Set_Speed(ball.Get_Speed() * AsConfig::Ball_Acceleration);
}
//-----------------------------------------------------------------------------------------------------------
void AsBall_Set::Reset_Speed()
{
	for (auto &ball : Balls)
		if (ball.Get_State() == EBall_State::Normal)
			ball.Set_Speed(AsConfig::Ball_Normal_Speed);
}
//-----------------------------------------------------------------------------------------------------------
void AsBall_Set::Forced_Advance(double offset)
{
	for (auto &ball : Balls)
		if (ball.Get_State() == EBall_State::On_Glue_Platform)
			ball.Forced_Advance(offset);
}
//-----------------------------------------------------------------------------------------------------------
bool AsBall_Set::Get_Next_Obj(int &index, AGame_Object **game_obj)
{
	if (index < 0 || index >= (int)Balls.size() )
		return false;

	*game_obj = &Balls[index++];

	return true;
}
//------------------------------------------------------------------------------------------------------------
