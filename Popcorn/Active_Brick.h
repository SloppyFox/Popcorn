#pragma once

#include "Config.h"

//------------------------------------------------------------------------------------------------------------
enum class EBrick_Type: unsigned char
{
	None,

	Red,
	Blue,
	Unbreakable,
	Multihit_1,
	Multihit_2,
	Multihit_3,
	Multihit_4,
	Parashute,
	Teleport,
	Ad,
	Invisible
};
//------------------------------------------------------------------------------------------------------------
enum class EDirection_Type: unsigned char
{
	Left,
	Up,
	Right,
	Down
};
//------------------------------------------------------------------------------------------------------------
class AAdvertisment: public AGraphics_Object
{
public:
	~AAdvertisment();
	AAdvertisment(int level_y, int level_x, int width, int height);

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	void Show_Under_Brick(int level_x, int level_y);

private:
	int Level_Y, Level_X;
	int Width, Height;
	int Ball_X, Ball_Y;
	int Ball_Width, Ball_Height;
	int Ball_Y_Offset, Ball_Y_Shift;
	double Acceleration_Step, Falling_Speed;
	double Deformation_Ratio;

	RECT Ad_Rect;
	HRGN Empty_Region;

	HRGN *Bricks_Regions;

	static const int Ball_Normal_Diameter = 12 * AsConfig::Global_Scale;
	static const int Max_Ball_Y_Offset = 7 * AsConfig::Global_Scale;
	static const int Min_Ball_Y_Offset = -5 * AsConfig::Global_Scale;
	static const int Deformation_Height = 3 * AsConfig::Global_Scale;
};
//------------------------------------------------------------------------------------------------------------
class AActive_Brick: public AGraphics_Object
{
public:
	void Get_Level_Pos(int &x_index, int &y_index) const;

protected:
	virtual ~AActive_Brick();
	AActive_Brick(EBrick_Type brick_type, int brick_y, int brick_x);

	virtual void Clear(HDC hdc, RECT &paint_area);

	double Get_Brick_X_Pos(bool is_center) const;
	double Get_Brick_Y_Pos(bool is_center) const;

	EBrick_Type Brick_Type;
	RECT Brick_Rect;

	int Level_Index_X, Level_Index_Y;
};
//------------------------------------------------------------------------------------------------------------
class AActive_Brick_Fading: public AActive_Brick
{
public:
	~AActive_Brick_Fading();
	AActive_Brick_Fading(EBrick_Type brick_type, int brick_y, int brick_x);

	virtual void Act();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	static void Setup_Colors();
	static void Draw_In_Level(HDC hdc, RECT &brick_rect, EBrick_Type brick_type);

private:
	int Fade_Step;

	static const int Max_Fade_Step = AsConfig::FPS;

	static AColor Fading_Red_Colors[Max_Fade_Step];
	static AColor Fading_Blue_Colors[Max_Fade_Step];
};
//------------------------------------------------------------------------------------------------------------
class AActive_Brick_Unbreakable: public AActive_Brick
{
public:
	~AActive_Brick_Unbreakable();
	AActive_Brick_Unbreakable(int brick_y, int brick_x);

	virtual void Act();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	static void Draw_In_Level(HDC hdc, RECT &brick_rect);

private:
	int Animation_Step;

	HRGN Highlight_Region;

	static const int Max_Animation_Step = 25;
};
//------------------------------------------------------------------------------------------------------------
class AActive_Brick_Multihit: public AActive_Brick
{
public:
	~AActive_Brick_Multihit();
	AActive_Brick_Multihit(int brick_y, int brick_x);

	virtual void Act();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	static void Draw_In_Level(HDC hdc, EBrick_Type brick_type, RECT &brick_rect);

private:
	static void Draw_Inner_Rects(HDC hdc, RECT &brick_rect, int x, int inner_width);


	bool Can_Dectroy;
	int Rotation_Step;

	static const int Max_Rotation_Step = 25;
};
//------------------------------------------------------------------------------------------------------------
enum class ETeleport_State: unsigned char
{
	Starting,
	Finishing,
	Done
};
//------------------------------------------------------------------------------------------------------------
class AActive_Brick_Teleport: public AActive_Brick
{
public:
	~AActive_Brick_Teleport();
	AActive_Brick_Teleport(int brick_y, int brick_x, ABall_Object *ball, AActive_Brick_Teleport *destination_teleport);

	virtual void Act();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	static void Draw_In_Level(HDC hdc, RECT &brick_rect, int step = 0);

	EDirection_Type Release_Direction;

private:
	ETeleport_State Teleport_State;
	ABall_Object *Ball;
	AActive_Brick_Teleport *Destination_Teleport;

	int Animation_Step;
	double Ball_X, Ball_Y;

	static const int Max_Animation_Step = 9;
};
//------------------------------------------------------------------------------------------------------------
class AActive_Brick_Ad: public AActive_Brick
{
public:
	~AActive_Brick_Ad();
	AActive_Brick_Ad(int brick_y, int brick_x, AAdvertisment *advertisment);

	virtual void Act();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	static void Draw_In_Level(HDC hdc, RECT &brick_rect);
};
//------------------------------------------------------------------------------------------------------------
