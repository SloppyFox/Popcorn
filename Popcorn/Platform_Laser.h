#pragma once

#include "Laser_Beam_Set.h"
#include "Platform_State.h"

//------------------------------------------------------------------------------------------------------------
enum class EFigure_Type: unsigned char
{
	Ellipse,
	Rectangle,
	Round_Rect_3x
};
//------------------------------------------------------------------------------------------------------------
class AsPlatform_Laser
{
public:
	~AsPlatform_Laser();
	AsPlatform_Laser(AsPlatform_State &platform_state);

	void Init(AColor &inner_color, AColor &circle_color, AsLaser_Beam_Set *laser_beams_set);
	bool Act(double platform_x_pos, EPlatform_State &next_state);
	void Draw_State(HDC hdc, double x_pos, RECT &curr_rect);
	void Fire(bool on_fire);

private:
	void Draw_Laser_Wing(HDC hdc, double x_pos, bool is_left);
	void Draw_Laser_Leg(HDC hdc, double x_pos, bool is_left);
	void Draw_Laser_Cabin(HDC hdc, double x_pos);
	double Get_Expanding_Value(double start_value, double end_value, double ratio);
	void Draw_Expanding_Figure(HDC hdc, EFigure_Type figure_type, double start_x, double start_y, double start_width, double start_height, double ratio, double end_x, double end_y, double end_width, double end_height);
	double Get_Gun_Pos(double platform_x_pos, bool is_left);

	AsPlatform_State *Platform_State;

	AsLaser_Beam_Set *Laser_Beams_Set;  // UNO

	AColor *Gun_Color;
	AColor *Inner_Color, *Circle_Color;  // UNO

	bool On_Fire;

	int Laser_Transformation_Step;
	int Laser_Finalize_Timer_Tick, Laser_Fire_Timer_Tick;

	static const int Max_Laser_Transformation_Step = 20;
};
//------------------------------------------------------------------------------------------------------------
