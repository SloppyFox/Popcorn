#pragma once

#include "Config.h"

enum class ELaser_Beam_State: unsigned char
{
	Disable,
	Active,
	Finalize
};
//------------------------------------------------------------------------------------------------------------
class ALaser_Beam: public AGame_Object
{
public:
	ALaser_Beam();

	virtual void Begin_Movement();
	virtual void Finish_Movement();
	virtual void Advance(double max_speed);
	virtual double Get_Speed();

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	void Set_On_Gun(double x_pos);
	ELaser_Beam_State Get_State() const;
	void Disable();

	static AHit_Checker_List Hit_Checker_List;

private:
	void Redraw_Beam();

	ELaser_Beam_State Laser_Beam_State;

	double X_Pos, Y_Pos;
	double Speed;

	RECT Beam_Rect, Prev_Beam_Rect;

	static const int Beam_Width = 1;
	static const int Beam_Height = 2;
};
//------------------------------------------------------------------------------------------------------------
