#pragma once

#include "Config.h"

//------------------------------------------------------------------------------------------------------------
class ALevel_Data
{
public:
	ALevel_Data(int level_number);

	char *Level;

	static const int Max_Level_Number = 4;

private:
	static char Test_Level[AsConfig::Level_Height][AsConfig::Level_Width];

	static char Level_01[AsConfig::Level_Height][AsConfig::Level_Width];
	static char Level_02[AsConfig::Level_Height][AsConfig::Level_Width];
	static char Level_03[AsConfig::Level_Height][AsConfig::Level_Width];
	static char Level_04[AsConfig::Level_Height][AsConfig::Level_Width];
	static char Level_05[AsConfig::Level_Height][AsConfig::Level_Width];
	static char Level_06[AsConfig::Level_Height][AsConfig::Level_Width];
	static char Level_07[AsConfig::Level_Height][AsConfig::Level_Width];
	static char Level_08[AsConfig::Level_Height][AsConfig::Level_Width];
	static char Level_09[AsConfig::Level_Height][AsConfig::Level_Width];
	static char Level_10[AsConfig::Level_Height][AsConfig::Level_Width];
};
//------------------------------------------------------------------------------------------------------------
