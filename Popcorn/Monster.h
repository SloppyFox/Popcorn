#pragma once

#include "Gate.h"
#include "Explosive_Ball.h"
#include "Level.h"

//------------------------------------------------------------------------------------------------------------
enum class EMonster_State: unsigned char
{
	Missing,
	Emitting,
	Alive,
	Destroing
};
//------------------------------------------------------------------------------------------------------------
class AMonster: public AHit_Checker, public AGame_Object
{
public:
	virtual ~AMonster();
	AMonster();
	
	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall_Object *ball);
	virtual bool Check_Hit(double next_x_pos, double next_y_pos);
	virtual bool Check_Hit(RECT &rect);

	virtual void Begin_Movement();
	virtual void Finish_Movement();
	virtual void Advance(double max_speed);
	virtual double Get_Speed();

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	void Let_Out(int gate_x_pos, int gate_y_pos, bool moving_right);
	void Destroy();
	void Set_Freeze_State(bool is_freeze);

protected:
	virtual void Act_Alive() = 0;
	virtual void Draw_Alive(HDC hdc) = 0;
	virtual void On_Activation() = 0;

	EMonster_State Monster_State;

	bool Need_To_Freeze;
	double X_Pos, Y_Pos;
	double Direction;
	double Prev_Speed;
	int Emiting_Timer_Tick;
	int Direction_Change_Timer_Tick;

	RECT Monster_Rect, Prev_Monster_Rect;

	static const int Monster_Size = 16;

private:	
	void Act_Destroing();
	void Draw_Destroing(HDC hdc, RECT &paint_area);
	void Get_Monster_Rect(double x_pos, double y_pos, RECT &rect);
	void Redraw_Monster();
	void Change_Direction();

	std::vector<AExplosive_Ball> Explosive_Balls;

	static const int Max_Explosive_Balls_Count = 20;
};
//------------------------------------------------------------------------------------------------------------
enum class EEye_State: unsigned char
{
	Closed,
	Opening,
	Staring,
	Closing
};
//------------------------------------------------------------------------------------------------------------
class AMonster_Eye: public AMonster
{
public:
	~AMonster_Eye();
	AMonster_Eye();

private:
	virtual void Act_Alive();
	virtual void Draw_Alive(HDC hdc);
	virtual void On_Activation();

	int Start_Blink_Timer_Tick;
	int Total_Animation_Timeout;

	double Cornea_Height;

	EEye_State Eye_State;

	std::vector<int> Blink_Ticks;

	static const double Max_Cornea_Height;

	static const int Blinks_Stages_Count = 7;

	static const double Blink_Timeouts[Blinks_Stages_Count];
	static const EEye_State Blink_States[Blinks_Stages_Count];

};
//------------------------------------------------------------------------------------------------------------
class AMonster_Comet: public AMonster
{
public:
	~AMonster_Comet();
	AMonster_Comet();

private:
	virtual void Clear(HDC hdc, RECT &paint_area);

	virtual void Act_Alive();
	virtual void Draw_Alive(HDC hdc);
	virtual void On_Activation();

	int Alive_Timer_Tick;
	int Ticks_Per_Rotation;

	double Current_Angle;

	static const int Min_Ticks_Per_Rotation = AsConfig::FPS * 2;
	static const int Max_Ticks_Per_Rotation = AsConfig::FPS / 2;
};
//------------------------------------------------------------------------------------------------------------
