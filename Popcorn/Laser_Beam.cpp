#include "Laser_Beam.h"

// ALaser_Beam
AHit_Checker_List ALaser_Beam::Hit_Checker_List;
//------------------------------------------------------------------------------------------------------------
ALaser_Beam::ALaser_Beam()
	: Laser_Beam_State(ELaser_Beam_State::Disable), X_Pos(0.0), Y_Pos(0.0), Speed(0.0), Beam_Rect{}, Prev_Beam_Rect{}
{

}
//------------------------------------------------------------------------------------------------------------
void ALaser_Beam::Begin_Movement()
{
	// не используется
}
//------------------------------------------------------------------------------------------------------------
void ALaser_Beam::Finish_Movement()
{
	if (Laser_Beam_State == ELaser_Beam_State::Disable)
		return;

	Redraw_Beam();
}
//------------------------------------------------------------------------------------------------------------
void ALaser_Beam::Advance(double max_speed)
{
	double step;

	if (Laser_Beam_State != ELaser_Beam_State::Active)
		return;

	step = Speed / max_speed * AsConfig::Moving_Step_Size;

	Y_Pos -= step;

	if (Hit_Checker_List.Check_Hit(X_Pos, Y_Pos) )
		Disable();

	if (Y_Pos < AsConfig::Level_Y_Offset)
		Disable();
}
//------------------------------------------------------------------------------------------------------------
double ALaser_Beam::Get_Speed()
{
	return Speed;
}
//------------------------------------------------------------------------------------------------------------
void ALaser_Beam::Act()
{
	//!!!
}
//------------------------------------------------------------------------------------------------------------
void ALaser_Beam::Clear(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if (Laser_Beam_State == ELaser_Beam_State::Disable)
		return;

	if (! IntersectRect(&intersection_rect, &paint_area, &Prev_Beam_Rect) )
		return;

	AsTools::Rect(hdc, Prev_Beam_Rect, AsConfig::BG_Color);

	if (Laser_Beam_State == ELaser_Beam_State::Finalize)
		Laser_Beam_State = ELaser_Beam_State::Disable;
}
//------------------------------------------------------------------------------------------------------------
void ALaser_Beam::Draw(HDC hdc, RECT &paint_area)
{
	int x, y;
	RECT intersection_rect;

	if (Laser_Beam_State != ELaser_Beam_State::Active)
		return;

	if (! IntersectRect(&intersection_rect, &paint_area, &Beam_Rect) )
		return;

	AsConfig::Laser_Color.Select_Pen(hdc);

	x = (int)(X_Pos * AsConfig::D_Global_Scale);
	y = (int)(Y_Pos * AsConfig::D_Global_Scale);

	MoveToEx(hdc, x + 1, y + 1, 0);
	LineTo(hdc, x + 1, y + Beam_Height * AsConfig::Global_Scale - 1);
}
//------------------------------------------------------------------------------------------------------------
bool ALaser_Beam::Is_Finished()
{
	return false;  // заглушка, т.к. лучи лазера не имеют анимации
}
//------------------------------------------------------------------------------------------------------------
void ALaser_Beam::Set_On_Gun(double x_pos)
{
	if (Laser_Beam_State != ELaser_Beam_State::Disable)
		return;

	Laser_Beam_State = ELaser_Beam_State::Active;

	X_Pos = x_pos;
	Y_Pos = AsConfig::Platform_Y_Pos - 1;

	Speed = 9.0;

	Redraw_Beam();
}
//------------------------------------------------------------------------------------------------------------
ELaser_Beam_State ALaser_Beam::Get_State() const
{
	return Laser_Beam_State;
}
//------------------------------------------------------------------------------------------------------------
void ALaser_Beam::Disable()
{
	Laser_Beam_State = ELaser_Beam_State::Finalize;
	Speed = 0.0;
}
//------------------------------------------------------------------------------------------------------------
void ALaser_Beam::Redraw_Beam()
{
	Prev_Beam_Rect = Beam_Rect;

	Beam_Rect.left = (int)( (X_Pos - (double)Beam_Width / 2.0) * AsConfig::D_Global_Scale);
	Beam_Rect.top = (int)(Y_Pos * AsConfig::D_Global_Scale);
	Beam_Rect.right = Beam_Rect.left + Beam_Width * AsConfig::Global_Scale + AsConfig::Global_Scale - 1;
	Beam_Rect.bottom = Beam_Rect.top + Beam_Height * AsConfig::Global_Scale + 1;

	AsTools::Invalidate_Rect(Prev_Beam_Rect);
	AsTools::Invalidate_Rect(Beam_Rect);
}
//------------------------------------------------------------------------------------------------------------
