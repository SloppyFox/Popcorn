#pragma once

#include "Config.h"

//------------------------------------------------------------------------------------------------------------
enum class EBall_State: unsigned char
{
	Disabled,

	Normal,
	Lost,
	On_Platform,
	On_Glue_Platform,
	On_Parashute,
	Off_Parashute,
	Teleporting_Stage_1,
	Teleporting_Stage_2
};
//------------------------------------------------------------------------------------------------------------
class ABall: public AGame_Object
{
public:
	ABall();

	virtual void Begin_Movement();
	virtual void Finish_Movement();
	virtual void Advance(double max_speed);
	virtual double Get_Speed();

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	void Set_Speed(double new_speed);
	void Set_For_Test();
	bool Is_Test_Finished();
	EBall_State Get_State() const;
	void Set_State(EBall_State new_state, double x_pos = 0, double y_pos = 0);
	double Get_Direction() const;
	void Set_Direction(double new_direction);
	void Reflect(bool from_horizontal);
	bool Is_Move_Up() const;
	bool Is_Move_Left() const;
	void Get_Center(double &x_pos, double &y_pos) const;
	void Set_On_Parashute(int brick_y, int brick_x);
	void Forced_Advance(double offset);
	void Draw_Teleporting(HDC hdc, int step) const;

	int Release_Timer_Tick;

	static const double Radius;

	static AHit_Checker_List Hit_Checker_List;

private:
	void Redraw_Ball();
	void Redraw_Parashute();
	void Draw_Parashute(HDC hdc, RECT &paint_area);
	void Clear_Parashute(HDC hdc);

	EBall_State Ball_State, Prev_Ball_State;

	double Ball_Speed, Prev_Ball_Speed;
	double Ball_Direction, Prev_Ball_Direction;

	bool Testing_Is_Active;
	int Test_Iteration;
	double Rest_Test_Distance;

	double Center_X_Pos, Center_Y_Pos;

	RECT Ball_Rect, Prev_Ball_Rect;
	RECT Parashute_Rect, Prev_Parashute_Rect;

	static const int Parashute_Size = 15;
};
//------------------------------------------------------------------------------------------------------------
