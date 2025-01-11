#pragma once

#include "Active_Brick.h"

//------------------------------------------------------------------------------------------------------------
enum class ELetter_Type: unsigned char
{
	C, // cansel action of letters 
	I, // inversion
	V, // velosity
	M, // monsters
	Plus, // life
	G, // glue
	W, // width

	F, // floor
	L, // laser
	T, // three balls

	N, // next level

	Count
};
//------------------------------------------------------------------------------------------------------------
enum class EFalling_Letter_State: unsigned char
{
	Normal,
	Have_To_Destroy,
	Destroed
};
//------------------------------------------------------------------------------------------------------------
class AFalling_Letter: AGraphics_Object
{
public:
	AFalling_Letter(EBrick_Type brick_type, ELetter_Type letter_type, int x, int y);

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	void Get_Cell(RECT &rect) const;
	void Destroy();
	void Show_Full_Size();

	static void Init();
	static ELetter_Type Get_Random_Letter_Type();

	bool Got_Hit;

	const EBrick_Type Brick_Type;
	const ELetter_Type Letter_Type;

private:
	void Set_Brick_Letter_Colors(bool is_switch_color, const AColor **front_color, const AColor **back_color);
	void Draw_Brick_Letter(HDC hdc);
	void Draw_Line(HDC hdc, int x_1, int y_1, int x_2, int y_2);

	EFalling_Letter_State Falling_Letter_State;

	bool Is_Lost;

	int X, Y;
	int Rotation_Step;
	int Next_Rotation_Tick;

	RECT Letter_Cell, Prev_Letter_Cell;

	static int Sum_Letters_Popularity;

	static const int Ticks_Per_Animation_Step = 4;
	static const int Max_Rotation_Step = 16;
	static const int Brick_Half_Height = AsConfig::Brick_Height * AsConfig::Global_Scale / 2;

	static const int Letters_Popularity[ (int)ELetter_Type::Count];
};
//------------------------------------------------------------------------------------------------------------
