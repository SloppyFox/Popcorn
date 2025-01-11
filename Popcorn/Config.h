#pragma once

#include "Common.h"

//------------------------------------------------------------------------------------------------------------
class AColor
{
public:
	~AColor();
	AColor();
	AColor(unsigned char r, unsigned char g, unsigned char b);
	AColor(const AColor &color, int pen_tickness);
	AColor(unsigned char r, unsigned char g, unsigned char b, int pen_tickness);
	AColor(const AColor &pen_color, const AColor &brush_color, int pen_tickness);

	void operator = (const AColor &another);
	void Set_As(unsigned char r, unsigned char g, unsigned char b);
	int Get_RGB() const;
	void Select(HDC hdc) const;
	void Select_Pen(HDC hdc) const;
	HBRUSH Get_Brush() const;

	unsigned char R, G, B;

private:
	HPEN Pen;
	HBRUSH Brush;
};
//------------------------------------------------------------------------------------------------------------
class AsConfig
{
public:
	static bool Level_Has_Floor;
	static int Current_Timer_Tick;
	static const AColor BG_Color, Red_Color, Blue_Color, White_Color, Symbol_Color;
	static const AColor Teleport_Color, Red_Highlight, Blue_Highlight, Laser_Color;
	static const AColor Gate_Highlight_Color, Monster_Comet_Tail;
	static const AColor Cornea_Color, Iris_Color, Explode_Outline_Color, Floor_Indicator_Color;
	static HWND Hwnd;

	static const double D_Global_Scale;
	static const double Moving_Step_Size;
	static const double Start_Ball_Y_Pos;
	static const double Max_Ball_Speed;
	static const double Ball_Normal_Speed;
	static const double Ball_Acceleration;
	static const double Min_Ball_Angle;
	static const double Ball_Radius;

	static const int Global_Scale = 3;
	static const int Platform_Y_Pos = 185;
	static const int Level_X_Offset = 8;
	static const int Level_Y_Offset = 8;
	static const int Level_Height = 14;
	static const int Level_Width = 12;
	static const int Space_Btw_Bricks = 1;
	static const int Brick_Width = 15;
	static const int Brick_Height= 7;
	static const int Cell_Width = Brick_Width + Space_Btw_Bricks;
	static const int Cell_Height = Brick_Height + Space_Btw_Bricks;
	static const int Bricks_Area_X_Offset = Level_X_Offset + Space_Btw_Bricks;
	static const int Bricks_Area_Y_Offset = Level_Y_Offset + Space_Btw_Bricks;
	static const int Level_Max_X_Offset = Bricks_Area_X_Offset + Cell_Width * Level_Width;
	static const int Play_Area_Max_Y_Offset = 200;
	static const int FPS = 30;
	static const int Max_Active_Bricks_Count = 10;
	static const int Max_Falling_Letters_count = 10;
	static const int Hits_Per_Letter = 1;  // Шанс выпадения "буквы" из кирпича -> 1.0 / Hits_Per_Letter
	static const int Max_Balls_Count = 10;
	static const int Max_Modules_Count = 10;
	static const int Max_Laser_Beams_Count = 10;
	static const int Max_Monsters_Count = 3;
	static const int Initial_Life_Count = 5;
	static const int Max_Life_Count = 12;
	static const int Floor_Y_Pos = Play_Area_Max_Y_Offset - 1;
	static const int Ball_On_Glue_Timeout = 60 * FPS;  //!!! 3 sec
	static const int Platform_Glue_Timeout = 60 * FPS;  //!!! no tested value, mb 30 sec
	static const int Platform_Expanded_Timeout = 60 * FPS;  //!!! no tested value
	static const int Platform_Laser_Timeout = 60 * FPS;  //!!! no tested value
	static const int Laser_Fire_Timeout = FPS / 2;
	static const int Gate_Short_Open_Timeout = FPS / 2;
	static const int Gate_Long_Open_Timeout = FPS * 3;
	static const int Platform_Normal_Width = 28;
	static const int Platform_Height = 7;
	static const int Platform_Circle_Diameter = Platform_Height;
	static const int Gates_Count = 8;
};
//------------------------------------------------------------------------------------------------------------
class AsTools
{
public:
	static int Rand(int range);
	static void Round_Rect(HDC hdc, RECT &rect, int rounding_value = 2);
	static void Round_Rect(HDC hdc, int x, int y, int width, int height, const AColor &color, int rounding_value = 2);
	static void Rect(HDC hdc, RECT &rect, const AColor &color);
	static void Rect(HDC hdc, int x, int y, int width, int height, const AColor &color);
	static void Ellipse(HDC hdc, RECT &rect, const AColor &color);
	static void Ellipse(HDC hdc, int x, int y, int width, int height, const AColor &color);
	static void Invalidate_Rect(RECT &rect);
	static unsigned char Get_Fading_Channel(unsigned char channel, unsigned char bg_channel, int step, int steps_count);
	static void Get_Fading_Color(const AColor &origin_color, int step, AColor &result_color, int max_step);
	static bool Reflect_From_Circle(double next_x_pos, double next_y_pos, double circle_radius, double circle_x, double circle_y, ABall_Object *ball);

	static void Throw();
};
//------------------------------------------------------------------------------------------------------------
class AHit_Checker
{
public:
	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall_Object *ball) = 0;
	virtual bool Check_Hit(double next_x_pos, double next_y_pos);
	virtual bool Check_Hit(RECT &rect);

	virtual bool Hit_Circle_On_Line(double distance_to_line, double ball_center_pos, double line_start_pos, double line_end_pos, double radius, double &x);
};
//------------------------------------------------------------------------------------------------------------
class AHit_Checker_List
{
public:
	AHit_Checker_List();

	void Add_Hit_Checker(AHit_Checker *hit_checker);
	bool Check_Hit(double x_pos, double y_pos);
	bool Check_Hit(double x_pos, double y_pos, ABall_Object *ball);
	bool Check_Hit(RECT &rect);

private:
	std::vector<AHit_Checker *> Hit_Checkers;
};
//------------------------------------------------------------------------------------------------------------
