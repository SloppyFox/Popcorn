#include "Border.h"

// AsBorder
//------------------------------------------------------------------------------------------------------------
AsBorder::~AsBorder()
{
	for (auto *gate : Gates)
		delete gate;

	Gates.erase(Gates.begin(), Gates.end() );
}
//------------------------------------------------------------------------------------------------------------
AsBorder::AsBorder()
	: Clear_Floor(false), Floor_Rect{}
{
	Floor_Rect.left = AsConfig::Level_X_Offset * AsConfig::Global_Scale;
	Floor_Rect.top = AsConfig::Floor_Y_Pos * AsConfig::Global_Scale;
	Floor_Rect.right = AsConfig::Level_Max_X_Offset * AsConfig::Global_Scale;
	Floor_Rect.bottom = AsConfig::Play_Area_Max_Y_Offset * AsConfig::Global_Scale;

	Gates.push_back(new AGate(2, 29, 0, 3) );
	Gates.push_back(new AGate(AsConfig::Level_Max_X_Offset, 29, AsConfig::Level_Width - 1, 3) );

	Gates.push_back(new AGate(2, 77, 0, 9) );
	Gates.push_back(new AGate(AsConfig::Level_Max_X_Offset, 77, AsConfig::Level_Width - 1, 9) );

	Gates.push_back(new AGate(2, 129) );
	Gates.push_back(new AGate(AsConfig::Level_Max_X_Offset, 129) );

	Gates.push_back(new AGate(2, 178) );
	Gates.push_back(new AGate(AsConfig::Level_Max_X_Offset, 178) );

	if (Gates.size() != AsConfig::Gates_Count)
		AsTools::Throw();
}
//------------------------------------------------------------------------------------------------------------
bool AsBorder::Check_Hit(double next_x_pos, double next_y_pos, ABall_Object *ball)
{// Проверяем столкновения с рамками уровня или полом
	bool got_hit = false;

	// 1. С левой или правой рамки
	if (next_x_pos - AsConfig::Ball_Radius < Left_Border_X_Offset || next_x_pos + AsConfig::Ball_Radius > Right_Border_X_Offset)
	{
		got_hit = true;
		ball->Reflect(false);
	}

	// 2. С верхней рамкой
	if (next_y_pos - AsConfig::Ball_Radius < Top_Border_X_Offset)
	{
		got_hit = true;
		ball->Reflect(true);
	}
	else if (next_y_pos + AsConfig::Ball_Radius >= AsConfig::Floor_Y_Pos && AsConfig::Level_Has_Floor)
	{
		got_hit = true;
		ball->Reflect(true);
	}
	else if (next_y_pos - AsConfig::Ball_Radius > AsConfig::Play_Area_Max_Y_Offset)
	{
		ball->Set_State(EBall_State::Lost);
	}

	return got_hit;
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Begin_Movement()
{
	// Заглушка т.к. этот метод не используется
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Finish_Movement()
{
	// Заглушка т.к. этот метод не используется
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Advance(double max_speed)
{
	// Заглушка т.к. этот метод не используется
}
//------------------------------------------------------------------------------------------------------------
double AsBorder::Get_Speed()
{
	return 0.0;  // Заглушка т.к. этот метод не используется
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Act()
{
	for (auto *gate : Gates)
		gate->Act();
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Clear(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	for (auto *gate : Gates)
		gate->Clear(hdc, paint_area);

	if (Clear_Floor)
	{
		if (IntersectRect(&intersection_rect, &paint_area, &Floor_Rect))
			AsTools::Rect(hdc, Floor_Rect, AsConfig::BG_Color);

		Clear_Floor = false;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Draw(HDC hdc, RECT &paint_area)
{
	Draw_Bounds(hdc, paint_area);

	if (AsConfig::Level_Has_Floor)
		Draw_Floor(hdc, paint_area);

	for (auto *gate : Gates)
		gate->Draw(hdc, paint_area);
}
//------------------------------------------------------------------------------------------------------------
bool AsBorder::Is_Finished()
{
	return false;  // Заглушка. Этот метод не используется
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Redraw_Floor()
{
	Clear_Floor = true;
	AsTools::Invalidate_Rect(Floor_Rect);
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Open_Gate(int gate_index, bool is_short_open)
{
	if (gate_index != (int)Gates.size() - 1 && is_short_open)
		AsTools::Throw();

	if (gate_index >= 0 || gate_index < (int)Gates.size() )
		Gates[gate_index]->Open_Gate(is_short_open);
	else
		AsTools::Throw();
}
//------------------------------------------------------------------------------------------------------------
int AsBorder::Long_Open_Gate()
{
	int i;
	bool got_gate = false;
	int gate_index;
	AGate *curr_gate;
	RECT monster_rect;

	gate_index = AsTools::Rand(Gates.size() );

	for (i = 0; i < (int)Gates.size(); i++)
	{
		curr_gate = Gates[gate_index];

		if (gate_index != Gates.size() - 1)
			if (Is_Gate_Close(gate_index) )
			{			
				if (curr_gate->Level_X == -1 || curr_gate->Level_Y == -1)
				{
					got_gate = true;
					break;
				}

				AsLevel::Has_Brick_At(monster_rect);

				if ( ! AsLevel::Has_Brick_At(curr_gate->Level_X, curr_gate->Level_Y) &&
					! AsLevel::Has_Brick_At(curr_gate->Level_X, curr_gate->Level_Y + 1) &&
					! AsLevel::Has_Brick_At(curr_gate->Level_X, curr_gate->Level_Y - 1))
				{
					got_gate = true;
					break;
				}
			}

		++gate_index;

		if (gate_index >= (int)Gates.size() )
			gate_index = 0;
	}

	if (! got_gate)
		AsTools::Throw();

	Open_Gate(gate_index, false);

	return gate_index;
}
//------------------------------------------------------------------------------------------------------------
bool AsBorder::Is_Gate_Opened(int gate_index)
{
	if (gate_index >= 0 && gate_index < (int)Gates.size() )
		return Gates[gate_index]->Is_Gate_Open();
	else
	{
		AsTools::Throw();
		return false;
	}
}
//------------------------------------------------------------------------------------------------------------
bool AsBorder::Is_Gate_Close(int gate_index)
{
	if (gate_index < 0 || gate_index >= (int)Gates.size() )
	{
		AsTools::Throw();
		return false;
	}

	return Gates[gate_index]->Is_Gate_Close();
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Get_Gate_Pos(int gate_index, int &gate_x_pos, int &gate_y_pos)
{
	Gates[gate_index]->Get_Gate_Pos(gate_x_pos, gate_y_pos);
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Draw_Tile(HDC hdc, RECT &paint_area, int x, int y, bool is_top) const
{
	int gate_top_y, gate_low_y;
	RECT intersection_rect, tile_rect{};

	tile_rect.left = x * AsConfig::Global_Scale;
	tile_rect.top = y * AsConfig::Global_Scale;
	tile_rect.right = tile_rect.left + Tile_Size * AsConfig::Global_Scale;
	tile_rect.bottom  = tile_rect.top + Tile_Size * AsConfig::Global_Scale;

	if ( ! IntersectRect(&intersection_rect, &paint_area, &tile_rect) )
		return;

	if (! is_top)
	{
		for (auto *gate : Gates)
		{
			gate->Get_Y_Area(gate_top_y, gate_low_y);

			if (gate_top_y <= tile_rect.top && gate_low_y >= tile_rect.bottom)
				return;
		}
	}

	AsTools::Rect(hdc, tile_rect, AsConfig::Blue_Color);
	AsTools::Rect(hdc, x + 2, y + 2, 1, 1, AsConfig::BG_Color);

	if (! is_top)
		AsTools::Rect(hdc, x, y, 1, Tile_Size, AsConfig::White_Color);
	else
		AsTools::Rect(hdc, x, y, Tile_Size, 1, AsConfig::White_Color);
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Draw_Bounds(HDC hdc, RECT &paint_area) const
{

	for (int i = 0; i < AsConfig::Play_Area_Max_Y_Offset / Tile_Size + 1; i++)
	{
		Draw_Tile(hdc, paint_area, Left_Border_X_Offset - Tile_Size, 1 + Tile_Size * i, false);  // left bound
		Draw_Tile(hdc, paint_area, Right_Border_X_Offset, 1 + Tile_Size * i, false);  // right bound
	}
	// top bound
	for (int i = 0; i < AsConfig::Level_Max_X_Offset / Tile_Size; i++)
		Draw_Tile(hdc, paint_area, Tile_Size + Tile_Size * i, 0, true);
}
//------------------------------------------------------------------------------------------------------------
void AsBorder::Draw_Floor(HDC hdc, RECT &paint_area) const
{
	int i;
	int x, y;
	int line_len, gap_len;
	int stroke_len, strokes_count;
	RECT intersection_rect;

	if ( ! IntersectRect(&intersection_rect, &paint_area, &Floor_Rect) )
		return;

	line_len = 6 * AsConfig::Global_Scale;
	gap_len = 2 * AsConfig::Global_Scale;

	x = Floor_Rect.left + gap_len - 1;
	y = Floor_Rect.top + (Floor_Rect.bottom - Floor_Rect.top) / 2;

	stroke_len = line_len + gap_len;

	strokes_count = (Floor_Rect.right - Floor_Rect.left) / stroke_len;

	for (i = 0; i < strokes_count; i++)
	{
		AsConfig::Symbol_Color.Select_Pen(hdc);

		MoveToEx(hdc, x, y, 0);
		LineTo(hdc, x + line_len - 1, y);

		x += stroke_len;
	}
}
//------------------------------------------------------------------------------------------------------------
