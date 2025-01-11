#pragma once

#include "Laser_Beam.h"

//------------------------------------------------------------------------------------------------------------
class AsLaser_Beam_Set: public AGame_Objects_Set
{
public:
	AsLaser_Beam_Set();

	void Fire(double left_gun_x_pos, double right_gun_x_pos);
	void Disable_All();

private:
	virtual bool Get_Next_Obj(int &index, AGame_Object **game_obj);

	std::vector<ALaser_Beam> Laser_Beams;
};
//------------------------------------------------------------------------------------------------------------
