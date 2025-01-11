#pragma once

#include "Gate.h"
#include "Level.h"

//------------------------------------------------------------------------------------------------------------
class AsBorder: public AHit_Checker, public AGame_Object
{
public:
	~AsBorder();
	AsBorder();

	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall_Object *ball);

	virtual void Begin_Movement();
	virtual void Finish_Movement();
	virtual void Advance(double max_speed);
	virtual double Get_Speed();

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	void Redraw_Floor();
	void Open_Gate(int gate_index, bool is_short_open);
	int Long_Open_Gate();
	bool Is_Gate_Opened(int gate_index);
	bool Is_Gate_Close(int gate_index);
	void Get_Gate_Pos(int gate_index, int &gate_x_pos, int &gate_y_pos);

private:
	void Draw_Tile(HDC hdc, RECT &paint_area, int x, int y, bool is_top) const;
	void Draw_Bounds(HDC hdc, RECT &paint_area) const;
	void Draw_Floor(HDC hdc, RECT &paint_area) const;

	bool Clear_Floor;

	std::vector<AGate *> Gates;

	RECT Floor_Rect;

	static const int Tile_Size = 4;
	static const int Left_Border_X_Offset = AsConfig::Level_X_Offset - 1;
	static const int Right_Border_X_Offset = AsConfig::Level_Max_X_Offset + 1;
	static const int Top_Border_X_Offset = AsConfig::Level_Y_Offset - Tile_Size / 2;
};
//------------------------------------------------------------------------------------------------------------
