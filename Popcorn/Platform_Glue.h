#pragma once

#include "Platform_State.h"
#include "Ball_Set.h"

//------------------------------------------------------------------------------------------------------------
class AsPlatform_Glue
{
public:
	AsPlatform_Glue(AsPlatform_State &platform_state);

	bool Act(AsBall_Set *ball_set, EPlatform_State &next_state);
	void Draw_State(HDC hdc, int platform_x);

private:
	void Draw_Glue_Spot(HDC hdc, int platform_x, int spot_x_offset, int spot_width, int spot_height) const;

	AsPlatform_State *Platform_State;

	int Glue_Finalize_Timer_Tick;

	double Glue_Spot_Height_Ratio;

	static const double Max_Glue_Spot_Height_Ratio, Glue_Spot_Ratio_Step;
};
//------------------------------------------------------------------------------------------------------------
