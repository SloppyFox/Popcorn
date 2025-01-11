#include "Laser_Beam_Set.h"

// AsLaser_Beam_Set
//------------------------------------------------------------------------------------------------------------
AsLaser_Beam_Set::AsLaser_Beam_Set()
: Laser_Beams(AsConfig::Max_Laser_Beams_Count)
{
}
//------------------------------------------------------------------------------------------------------------
void AsLaser_Beam_Set::Fire(double left_gun_x_pos, double right_gun_x_pos)
{
	ALaser_Beam *left_beam = 0, *right_beam = 0;

	for (auto &curr_beam : Laser_Beams)
	{
		if (curr_beam.Get_State() != ELaser_Beam_State::Disable)
			continue;

		if (left_beam == 0)
			left_beam = &curr_beam;
		else if (right_beam == 0)
		{
			right_beam = &curr_beam;
			break;
		}
	}

	if (left_beam != 0)
		left_beam->Set_On_Gun(left_gun_x_pos);
	if (right_beam != 0)
		right_beam->Set_On_Gun(right_gun_x_pos);

	if (left_beam == 0 || right_beam == 0)
		AsTools::Throw();
}
//------------------------------------------------------------------------------------------------------------
void AsLaser_Beam_Set::Disable_All()
{
	for (auto &curr_beam : Laser_Beams)
		curr_beam.Disable();
}
//------------------------------------------------------------------------------------------------------------
bool AsLaser_Beam_Set::Get_Next_Obj(int &index, AGame_Object **game_obj)
{
	if (index < 0 || index >= (int)Laser_Beams.size() )
		return false;

	*game_obj = &Laser_Beams[index++];
	
	return true;
}
//------------------------------------------------------------------------------------------------------------
