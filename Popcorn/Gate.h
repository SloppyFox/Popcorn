#pragma once

#include "Config.h"

//------------------------------------------------------------------------------------------------------------
enum class EGate_State: unsigned char
{
	Closed,
	Short_Open,
	Long_Open
};
//------------------------------------------------------------------------------------------------------------
enum class EGate_Transformation: unsigned char
{
	Unknown,

	Init,
	Active,
	Finalize
};
//------------------------------------------------------------------------------------------------------------
class AGate: public AGraphics_Object
{
public:
	AGate(int x_pos, int y_pos, int level_x = -1, int level_y = -1);

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	void Open_Gate(bool is_short_open);
	bool Is_Gate_Open() const;
	bool Is_Gate_Close() const;
	void Get_Y_Area(int &gate_top_y, int &gate_low_y) const;
	void Get_Gate_Pos(int &gate_x_pos, int &gate_y_pos) const;

	const int Level_X, Level_Y;

	static const int Width = 6;

private:
	void Act_Opening(bool is_short, bool &correct_pos);
	void Draw_Cup(HDC hdc, bool is_top) const;
	void Draw_Part(HDC hdc, bool is_top);
	void Draw_Edges(HDC hdc);
	void Draw_Short_Open_Edges(HDC hdc);
	void Draw_Long_Open_Edges(HDC hdc);
	void Draw_Red_Edge(HDC hdc, int edges_y_offset, bool is_long_edge, bool has_highlight);
	void Draw_One_Edge(HDC hdc, int edges_y_offset, bool is_long_edge);
	void Draw_Charge(HDC hdc) const;
	void Redraw_Gate();

	EGate_State Gate_State;
	EGate_Transformation Gate_Transformation;

	int X_Pos;
	double Y_Pos, Origin_Y_Pos;
	int Gate_Open_Timer_Tick;

	double Gap_Height;

	RECT Gate_Rect;

	static const double Max_Gap_Short_Height, Opening_Short_Step;
	static const double Max_Gap_Long_Height, Opening_Long_Step;

	static const int Height = 19;
	static const int Edges_Count = 5;
};
//------------------------------------------------------------------------------------------------------------
