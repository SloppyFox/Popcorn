#pragma once

#include "Config.h"

//------------------------------------------------------------------------------------------------------------
enum class EExplosive_Ball_State: unsigned char
{
	Idle,
	Waiting,
	Expanding,
	Fading
};
//------------------------------------------------------------------------------------------------------------
class AExplosive_Ball: public AGraphics_Object
{
public:
	AExplosive_Ball();

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	void Explode(double x_pos, double y_pos, double max_size, int explode_delay_ticks, bool is_red);

	static void Setup_Colors();

private:
	void Update_Rect();
	void Act_Expanding_State();
	void Act_Fading_State();

	EExplosive_Ball_State Explosive_Ball_State;
	bool Is_Red;
	double X_Pos, Y_Pos;
	double Size, Max_Size;
	int Start_Explode_Tick, Start_Fading_Tick, End_Explode_Tick;

	RECT Curr_Rect;

	AColor *Curr_Ball_Color;

	static const int Expanding_Timeout = AsConfig::FPS / 2;
	static const int Fading_Timeout = AsConfig::FPS / 2;
	static const int Fading_Steps_Count = Fading_Timeout;

	static AColor Fading_Red_Colors[Fading_Steps_Count];
	static AColor Fading_Blue_Colors[Fading_Steps_Count];
};
//------------------------------------------------------------------------------------------------------------
