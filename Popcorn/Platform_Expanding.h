#pragma once

#include "Platform_State.h"

//------------------------------------------------------------------------------------------------------------
class AsPlatform_Expanding
{
public:
	~AsPlatform_Expanding();
	AsPlatform_Expanding(AsPlatform_State &platform_state);

	void Init(AColor &inner_color, AColor &circle_color, AColor &highlight_color);
	bool Act(double &x_pos, EPlatform_State &next_state);
	void Draw_State(HDC hdc, double x_pos);
	void Draw_Circle_Highlight(HDC hdc, int x, int y);
	void Reset();

	double Curr_Expanding_Width;

private:
	void Draw_Expanding_Part(HDC hdc, int x, RECT &inner_rect, bool is_left);

	AsPlatform_State *Platform_State;
	AColor *Truss_Color;
	AColor *Inner_Color, *Circle_Color, *Highlight_Color; // UNO

	int Expanding_Finalize_Timer_Tick;

	static const double Max_Expanding_Width, Min_Expanding_Width, Expanding_Width_Step;

	static const int Expanding_Inner_Width = 12;
};
//------------------------------------------------------------------------------------------------------------
