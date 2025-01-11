#include "Falling_Letter.h"

// AFalling_Letter
const int AFalling_Letter::Letters_Popularity[ (int)ELetter_Type::Count] = { 7, 7, 7, 7, 7, 7, 7,  3, 3, 3,  1 };
int AFalling_Letter::Sum_Letters_Popularity;
//------------------------------------------------------------------------------------------------------------
AFalling_Letter::AFalling_Letter(EBrick_Type brick_type, ELetter_Type letter_type, int x, int y)
	: Falling_Letter_State(EFalling_Letter_State::Normal), Got_Hit(false), Brick_Type(brick_type), Letter_Type(letter_type), Is_Lost(false), X(x * AsConfig::Global_Scale), Y(y * AsConfig::Global_Scale), Rotation_Step(0), Next_Rotation_Tick(AsConfig::Current_Timer_Tick + Ticks_Per_Animation_Step)
{
	Letter_Cell.left = X;
	Letter_Cell.top = Y;
	Letter_Cell.right = Letter_Cell.left + AsConfig::Brick_Width * AsConfig::Global_Scale;
	Letter_Cell.bottom = Letter_Cell.top + AsConfig::Brick_Height * AsConfig::Global_Scale;

	Prev_Letter_Cell = Letter_Cell;
}
//------------------------------------------------------------------------------------------------------------
void AFalling_Letter::Act()
{
	if (Falling_Letter_State != EFalling_Letter_State::Normal)
		return;

	if (Letter_Cell.top >= AsConfig::Play_Area_Max_Y_Offset * AsConfig::Global_Scale)
	{
		Destroy();
		return;
	}

	Prev_Letter_Cell = Letter_Cell;

	Y += AsConfig::Global_Scale;
	Letter_Cell.top += AsConfig::Global_Scale;
	Letter_Cell.bottom += AsConfig::Global_Scale;

	if (AsConfig::Current_Timer_Tick >= Next_Rotation_Tick)
	{
		Next_Rotation_Tick += Ticks_Per_Animation_Step;
		++Rotation_Step;
	}

	AsTools::Invalidate_Rect(Prev_Letter_Cell);
	AsTools::Invalidate_Rect(Letter_Cell);
}
//------------------------------------------------------------------------------------------------------------
void AFalling_Letter::Clear(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if (! IntersectRect(&intersection_rect, &paint_area, &Prev_Letter_Cell) )
		return;

	AsTools::Rect(hdc, Prev_Letter_Cell, AsConfig::BG_Color);
}
//------------------------------------------------------------------------------------------------------------
void AFalling_Letter::Draw(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if (Falling_Letter_State == EFalling_Letter_State::Have_To_Destroy)
	{
		Falling_Letter_State = EFalling_Letter_State::Destroed;
		return;
	}

	if (IntersectRect(&intersection_rect, &paint_area, &Letter_Cell) )
		Draw_Brick_Letter(hdc);
}
//------------------------------------------------------------------------------------------------------------
bool AFalling_Letter::Is_Finished()
{
	if (Falling_Letter_State == EFalling_Letter_State::Destroed)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
void AFalling_Letter::Get_Cell(RECT &rect) const
{
	rect = Letter_Cell;
}
//------------------------------------------------------------------------------------------------------------
void AFalling_Letter::Destroy()
{	
	Falling_Letter_State = EFalling_Letter_State::Have_To_Destroy;

	AsTools::Invalidate_Rect(Prev_Letter_Cell);
	AsTools::Invalidate_Rect(Letter_Cell);
}
//------------------------------------------------------------------------------------------------------------
void AFalling_Letter::Show_Full_Size()
{
	Rotation_Step = 8;
}
//------------------------------------------------------------------------------------------------------------
void AFalling_Letter::Init()
{
	int i;

	Sum_Letters_Popularity = 0;

	for (i = 0; i < (int)ELetter_Type::Count; i++)
		Sum_Letters_Popularity += Letters_Popularity[i];
}
//------------------------------------------------------------------------------------------------------------
ELetter_Type AFalling_Letter::Get_Random_Letter_Type()
{
	int i;
	int rand_letter_popularity;
	ELetter_Type letter_type;

	rand_letter_popularity = AsTools::Rand(Sum_Letters_Popularity);

	for (i = 0; i < (int)ELetter_Type::Count; i++)
	{
		if (rand_letter_popularity < Letters_Popularity[i])
		{
			letter_type = (ELetter_Type) i;
			return letter_type;
		}

		rand_letter_popularity -= Letters_Popularity[i];
	}

	return ELetter_Type::C;
}
//------------------------------------------------------------------------------------------------------------
void AFalling_Letter::Set_Brick_Letter_Colors(bool is_switch_color, const AColor **front_color, const AColor **back_color)
{
	if (is_switch_color)
	{
		*front_color = &AsConfig::Red_Color;
		*back_color = &AsConfig::Blue_Color;
	}
	else
	{
		*front_color = &AsConfig::Blue_Color;
		*back_color = &AsConfig::Red_Color;
	}
}
//------------------------------------------------------------------------------------------------------------
void AFalling_Letter::Draw_Brick_Letter(HDC hdc)
{
	bool switch_color;
	double offset;
	double rotation_angle;
	double y_ratio;
	int back_part_offset;
	int center_letter_x, left_letter_x, right_letter_x, top_letter_y, bottom_letter_y;
	const int scale = AsConfig::Global_Scale;
	const AColor *front_color, *back_color;
	XFORM xform{}, old_xform;

	if (!(Brick_Type == EBrick_Type::Blue || Brick_Type == EBrick_Type::Red))
		return;

	Rotation_Step = Rotation_Step % Max_Rotation_Step;

	if (Rotation_Step < 8)
		rotation_angle = 2.0 * M_PI / (double)Max_Rotation_Step * (double)Rotation_Step;
	else
		rotation_angle = 2.0 * M_PI / (double)Max_Rotation_Step * (double)(8 - Rotation_Step);

	if (Rotation_Step > 4 && Rotation_Step <= 12)
	{
		if (Brick_Type == EBrick_Type::Blue)
			switch_color = true;
		else
			switch_color = false;
	}
	else
	{
		if (Brick_Type == EBrick_Type::Red)
			switch_color = true;
		else
			switch_color = false;
	}

	Set_Brick_Letter_Colors(switch_color, &front_color, &back_color);


	if (Rotation_Step == 4 || Rotation_Step == 12)
	{
		back_color->Select(hdc);
		RoundRect(hdc, X, Y + Brick_Half_Height - scale, X + AsConfig::Brick_Width * scale - 1, Y + Brick_Half_Height, 2 * scale, 3 * scale);

		front_color->Select(hdc);
		RoundRect(hdc, X, Y + Brick_Half_Height, X + AsConfig::Brick_Width * scale - 1, Y + Brick_Half_Height + scale - 1, 2 * scale, 3 * scale);
	}
	else
	{
		y_ratio = cos(rotation_angle);

		xform.eM11 = 1.0f;
		xform.eM12 = 0.0f;
		xform.eM21 = 0.0f;
		xform.eM22 = (float)fabs(y_ratio);
		xform.eDx = (float)X;
		xform.eDy = (float)Y + (float)(Brick_Half_Height);
		GetWorldTransform(hdc, &old_xform);
		SetWorldTransform(hdc, &xform);

		offset = 3.0 * (1.0 - fabs(xform.eM22) ) * AsConfig::D_Global_Scale;
		back_part_offset = (int)round(offset);

		if (y_ratio < 0.0)
			back_part_offset = -back_part_offset;

		back_color->Select(hdc);
		RoundRect(hdc, 0, -Brick_Half_Height - back_part_offset, AsConfig::Brick_Width * scale - 1, Brick_Half_Height - back_part_offset, 2 * scale, 3 * scale);

		front_color->Select(hdc);
		RoundRect(hdc, 0, -Brick_Half_Height, AsConfig::Brick_Width * scale - 1, Brick_Half_Height, 2 * scale, 2 * scale);

		if (Rotation_Step > 4 && Rotation_Step <= 12)
		{
			AsConfig::Symbol_Color.Select_Pen(hdc);

			center_letter_x = 7 * scale + 1;
			left_letter_x = 5 * scale;
			right_letter_x = 10 * scale - 1;
			top_letter_y = 1 * scale - Brick_Half_Height;
			bottom_letter_y = 6 * scale - Brick_Half_Height - 1;

			switch(Letter_Type)
			{
			case ELetter_Type::C:
				Arc(hdc, left_letter_x, top_letter_y, right_letter_x + 1, bottom_letter_y, right_letter_x, top_letter_y - 2 * scale, right_letter_x, bottom_letter_y + 3 * scale);
				break;

			case ELetter_Type::I:
				Draw_Line(hdc, center_letter_x, top_letter_y, center_letter_x, bottom_letter_y);
				Draw_Line(hdc, center_letter_x - 2 * scale, top_letter_y, center_letter_x + 2 * scale, top_letter_y);
				Draw_Line(hdc, center_letter_x - 2 * scale, bottom_letter_y, center_letter_x + 2 * scale, bottom_letter_y);
				break;

			case ELetter_Type::V:
				Draw_Line(hdc, left_letter_x, top_letter_y, center_letter_x, bottom_letter_y);
				LineTo(hdc, right_letter_x, top_letter_y);
				break;

			case ELetter_Type::M:
				Draw_Line(hdc, left_letter_x, bottom_letter_y, left_letter_x, top_letter_y);
				LineTo(hdc, center_letter_x, 0);
				LineTo(hdc, right_letter_x, top_letter_y);
				LineTo(hdc, right_letter_x, bottom_letter_y);
				break;

			case ELetter_Type::Plus:
				Draw_Line(hdc, center_letter_x, top_letter_y, center_letter_x, bottom_letter_y);
				Draw_Line(hdc, left_letter_x, 0, right_letter_x, 0);
				break;

			case ELetter_Type::L:
				Draw_Line(hdc, left_letter_x, top_letter_y, left_letter_x, bottom_letter_y);
				LineTo(hdc, right_letter_x, bottom_letter_y);
				break;

			case ELetter_Type::G:
				Arc(hdc, left_letter_x, top_letter_y, right_letter_x + 1, bottom_letter_y, right_letter_x, top_letter_y - 2 * scale, right_letter_x, bottom_letter_y + 2 * scale);
				Draw_Line(hdc, right_letter_x - scale, bottom_letter_y, right_letter_x - scale, 0);
				break;

			case ELetter_Type::W:
				Draw_Line(hdc, left_letter_x, top_letter_y, left_letter_x, bottom_letter_y);
				LineTo(hdc, center_letter_x, 0);
				LineTo(hdc, right_letter_x, bottom_letter_y);
				LineTo(hdc, right_letter_x, top_letter_y);
				break;

			case ELetter_Type::F:
				Draw_Line(hdc, left_letter_x + scale, bottom_letter_y, left_letter_x + scale, top_letter_y);
				LineTo(hdc, right_letter_x, top_letter_y);
				Draw_Line(hdc, left_letter_x + scale, 0, center_letter_x + scale, 0);
				break;

			case ELetter_Type::T:
				Draw_Line(hdc, center_letter_x, bottom_letter_y, center_letter_x, top_letter_y);
				Draw_Line(hdc, left_letter_x, top_letter_y, right_letter_x, top_letter_y);
				break;

			case ELetter_Type::N:
				Draw_Line(hdc, left_letter_x, bottom_letter_y, left_letter_x, top_letter_y);
				LineTo(hdc, right_letter_x, bottom_letter_y);
				LineTo(hdc, right_letter_x, top_letter_y);
				break;

			default:
				break;
			}
		}

		SetWorldTransform(hdc, &old_xform);
	}
}
//------------------------------------------------------------------------------------------------------------
void AFalling_Letter::Draw_Line(HDC hdc, int x_1, int y_1, int x_2, int y_2)
{
	MoveToEx(hdc, x_1, y_1, 0);
	LineTo(hdc, x_2, y_2);
}
//------------------------------------------------------------------------------------------------------------
