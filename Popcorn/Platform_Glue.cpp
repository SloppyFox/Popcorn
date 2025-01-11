#include "Platform_Glue.h"

// AsPlatform_Glue
const double AsPlatform_Glue::Max_Glue_Spot_Height_Ratio = 1.0;
const double AsPlatform_Glue::Glue_Spot_Ratio_Step = 0.02;
//------------------------------------------------------------------------------------------------------------
AsPlatform_Glue::AsPlatform_Glue(AsPlatform_State &platform_state)
	: Platform_State(&platform_state), Glue_Finalize_Timer_Tick(0), Glue_Spot_Height_Ratio(0.0)
{

}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform_Glue::Act(AsBall_Set *ball_set, EPlatform_State &next_state)
{
	next_state = EPlatform_State::Unknown;

	switch (Platform_State->Glue)
	{
	case EPlatform_Transformation::Init:
		if (Glue_Spot_Height_Ratio < Max_Glue_Spot_Height_Ratio)
		{
			Glue_Spot_Height_Ratio += Glue_Spot_Ratio_Step;
			return true;
		}
		else
		{
			Platform_State->Glue = EPlatform_Transformation::Active;
			Glue_Finalize_Timer_Tick = AsConfig::Current_Timer_Tick + AsConfig::Platform_Glue_Timeout;
		}
		break;


	case EPlatform_Transformation::Active:
		if (AsConfig::Current_Timer_Tick == Glue_Finalize_Timer_Tick)
			Platform_State->Glue = EPlatform_Transformation::Finalize;
		break;


	case EPlatform_Transformation::Finalize:
		if (Glue_Spot_Height_Ratio > 0)
		{
			Glue_Spot_Height_Ratio -= Glue_Spot_Ratio_Step;
			while(ball_set->Release_From_Glue_Platform() )
			{}

			return true;
		}
		else
		{
			Platform_State->Glue = EPlatform_Transformation::Unknown;
			next_state = Platform_State->Set_State(EPlatform_Substate_Regular::Normal);
		}
		break;

	default:
		AsTools::Throw();
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Glue::Draw_State(HDC hdc, int platform_x)
{
	RECT rect{};
	HRGN region;

	rect.left = (platform_x + 4) * AsConfig::Global_Scale;
	rect.top = (AsConfig::Platform_Y_Pos + 1) * AsConfig::Global_Scale;
	rect.right = rect.left + AsConfig::Platform_Normal_Width * AsConfig::Global_Scale;
	rect.bottom = (AsConfig::Platform_Y_Pos - 1 + AsConfig::Platform_Circle_Diameter) * AsConfig::Global_Scale;

	region = CreateRectRgnIndirect(&rect);

	SelectClipRgn(hdc, region);

	Draw_Glue_Spot(hdc, platform_x, 0, 9, 4);
	Draw_Glue_Spot(hdc, platform_x, 6, 5, 5);
	Draw_Glue_Spot(hdc, platform_x, 9, 10, 6);

	SelectClipRgn(hdc, 0);
	DeleteObject(region);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform_Glue::Draw_Glue_Spot(HDC hdc, int platform_x, int spot_x_offset, int spot_width, int spot_height) const
{
	const int scale = AsConfig::Global_Scale;
	const double d_scale = AsConfig::D_Global_Scale;
	const int spot_x_pos = 5 * scale;
	const int y_pos = AsConfig::Platform_Y_Pos * scale;
	RECT spot_rect{};

	spot_rect.left = platform_x * scale + spot_x_pos + spot_x_offset * scale;
	spot_rect.top = (int)( (double)y_pos - (double)spot_height * Glue_Spot_Height_Ratio * d_scale / 2.0);
	spot_rect.right = spot_rect.left + spot_width * scale;
	spot_rect.bottom = (int)( (double)y_pos + (double)spot_height * Glue_Spot_Height_Ratio * d_scale);

	AsConfig::BG_Color.Select(hdc);

	Chord(hdc, spot_rect.left + 1, spot_rect.top, spot_rect.right - 1, spot_rect.bottom - 1, 
		spot_rect.left, y_pos + scale, spot_rect.right, y_pos + scale);

	AsConfig::White_Color.Select(hdc);

	Chord(hdc, spot_rect.left, spot_rect.top, spot_rect.right - scale, spot_rect.bottom - scale, 
		spot_rect.left, y_pos + scale, spot_rect.right, y_pos + scale);
}
//------------------------------------------------------------------------------------------------------------
