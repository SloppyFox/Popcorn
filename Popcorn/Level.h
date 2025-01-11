#pragma once

#include "Falling_Letter.h"
#include "Info_Panel.h"
#include "Level_Data.h"

//------------------------------------------------------------------------------------------------------------
class APoint
{
public:
	APoint();
	APoint(int x, int y);

	int X, Y;
};
//------------------------------------------------------------------------------------------------------------
class AsLevel: public AHit_Checker, public AGame_Object
{
public:
	~AsLevel();
	AsLevel();

	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall_Object *ball);
	virtual bool Check_Hit(double next_x_pos, double next_y_pos);

	virtual void Begin_Movement();
	virtual void Finish_Movement();
	virtual void Advance(double max_speed);
	virtual double Get_Speed();

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	void Init();
	void Set_Current_Level(int level_number);
	bool Get_Falling_Letter(int &index, AFalling_Letter **falling_letter) const;
	void Stop();

	static bool Has_Brick_At(int level_x, int level_y);
	static bool Has_Brick_At(RECT &rect);

private:
	bool On_Hit(int brick_y, int brick_x, ABall_Object *ball, bool vertical_hit);
	void On_Hit(int level_y, int level_x);
	bool Create_Active_Brick(int brick_y, int brick_x, EBrick_Type brick_type, ABall_Object *ball, bool vertical_hit);
	void Add_Active_Brick_Teleport(int source_x, int source_y, ABall_Object *ball, bool vertical_hit);
	bool Add_Falling_Letter(int brick_y, int brick_x, EBrick_Type brick_type);
	AActive_Brick_Teleport *Select_Destination_Teleport(int source_y, int source_x);
	bool Check_Vertical_Hit(double next_x_pos, double next_y_pos, int level_x, int level_y, double &reflection_pos, ABall_Object *ball);
	bool Check_Horizontal_Hit(double next_x_pos, double next_y_pos, int level_x, int level_y, double &reflection_pos, ABall_Object *ball);
	void Draw_Brick(HDC hdc, RECT &brick_rect,int level_x, int level_y);
	void Draw_Parashute_Brick_In_Level(HDC hdc, RECT &brick_rect);
	void Draw_Parashute_Segment(HDC hdc, RECT &brick_rect, int x_offset, int segment_width);
	void Redraw_Brick(int brick_y, int brick_x);
	void Act_Object(std::vector<AGraphics_Object *> &graphics_object);
	void Delete_Object(std::vector<AGraphics_Object *> &graphics_object);
	void Cansel_All_Activity();

	AColor Parashute_Brick_Color;
	AAdvertisment *Advertisment;

	bool Need_To_Cansel_All;

	double Current_Brick_Left_X, Current_Brick_Right_X;
	double Current_Brick_Top_Y, Current_Brick_Low_Y;

	RECT Level_Rect;
	std::vector<APoint> Teleport_Bricks_Pos;

	std::vector<AGraphics_Object *> Active_Bricks;
	std::vector<AGraphics_Object *> Falling_Letters;

	char Current_Level[AsConfig::Level_Height][AsConfig::Level_Width];
	std::vector<ALevel_Data *> Levels_Data;

	static AsLevel *Level;
};
//------------------------------------------------------------------------------------------------------------
