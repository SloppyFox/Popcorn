#pragma once

#include "Falling_Letter.h"
#include "Platform_Glue.h"
#include "Platform_Expanding.h"
#include "Platform_Laser.h"

//------------------------------------------------------------------------------------------------------------
class AsPlatform: public AHit_Checker, public AGame_Object
{
public:
	AsPlatform();

	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall_Object *ball);

	virtual void Begin_Movement();
	virtual void Finish_Movement();
	virtual void Advance(double max_speed);
	virtual double Get_Speed();

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	void Init(AsBall_Set *ball_set, AsLaser_Beam_Set *laser_beams_set);
	void Redraw();
	EPlatform_State Get_State() const;
	EPlatform_Moving_State Get_Moving_State() const;
	double Get_Moving_Direction() const;
	void Set_State(EPlatform_State new_state);
	bool Has_State(EPlatform_Substate_Regular regular_state) const;
	void Set_State(EPlatform_Substate_Regular new_regular_substate);
	void Move(bool to_left, bool key_down);
	bool Hit_By(AFalling_Letter *falling_letter) const;
	double Get_Middle_Pos() const;
	double Get_X_Offset() const;
	void On_Space_Key(bool key_down);

	static AHit_Checker_List Hit_Checker_List;

private:
	bool Set_Transformation_State(EPlatform_State new_state, EPlatform_Transformation &transformation_state);
	void Act_Rolling_State();
	void Act_Meltdown_State();
	void Draw_Rolling_State(HDC hdc);
	void Draw_Roll_In_Substate(HDC hdc);
	void Draw_Normal_State(HDC hdc);
	void Draw_Meltdown_State(HDC hdc, RECT &paint_area);
	double Get_Current_Width() const;
	bool Correct_Platform_Pos();

	AsPlatform_State Platform_State;
	AsBall_Set *Ball_Set;
	AsPlatform_Glue Platform_Glue;
	AsPlatform_Expanding Platform_Expanding;
	AsPlatform_Laser Platform_Laser;

	bool Left_Key_Down, Right_Key_Down;

	int Current_Redraw_Timer_Tick;
	int Inner_Width;
	int Rolling_Step;

	double X_Pos, Prev_X_Pos;

	AColor Circle_Color, Inner_Color, Highlight_Color;

	RECT Prev_Rect, Curr_Rect;

	static const double Platform_Normal_Speed;

	static const int Max_Rolling_Step = 16;
	static const int Roll_In_Platform_End_X_Pos = 101;
	static const int Rolling_Platform_Speed = 3;
	static const int Meltdown_Speed = 3;
	static const int Normal_Inner_Width = AsConfig::Platform_Normal_Width - AsConfig::Platform_Circle_Diameter;
};
//------------------------------------------------------------------------------------------------------------
