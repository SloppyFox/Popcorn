#include "Active_Brick.h"

// AAdvertisment
//------------------------------------------------------------------------------------------------------------
AAdvertisment::~AAdvertisment()
{
	int i, j;
	HRGN region;

	for (i = 0; i < Height; i++)
		for (j = 0; j < Width; j++)
		{
			region = Bricks_Regions[i * Width + j];

			if (region != 0)
				DeleteObject(region);
		}

	delete[] Bricks_Regions;
}
//------------------------------------------------------------------------------------------------------------
AAdvertisment::AAdvertisment(int level_y, int level_x, int width, int height)
	: Level_Y(level_y), Level_X(level_x), Width(width), Height(height), Ball_X(0), Ball_Y(0), Ball_Width(Ball_Normal_Diameter), Ball_Height(Ball_Normal_Diameter),
	Ball_Y_Offset(0), Ball_Y_Shift(1), Acceleration_Step(0.2), Falling_Speed(0.0), Deformation_Ratio(0.0), Ad_Rect{}, Empty_Region(0), Bricks_Regions(0)
{
	const int scale = AsConfig::Global_Scale;

	Empty_Region = CreateRectRgn(0, 0, 0, 0);

	Bricks_Regions = new HRGN[Height * Width];
	memset(Bricks_Regions, 0, sizeof(HRGN) * char(Height * Width) );

	Ad_Rect.left = (AsConfig::Bricks_Area_X_Offset + Level_X * AsConfig::Cell_Width) * scale;
	Ad_Rect.top = (AsConfig::Bricks_Area_Y_Offset + Level_Y * AsConfig::Cell_Height) * scale;
	Ad_Rect.right = Ad_Rect.left + ( (Width - 1) * AsConfig::Cell_Width + AsConfig::Brick_Width) * scale;
	Ad_Rect.bottom = Ad_Rect.top + ( (Height - 1) * AsConfig::Cell_Height + AsConfig::Brick_Height) * scale;

	Ball_X = Ad_Rect.left + Ball_Normal_Diameter / 2 + 9 * scale + 1;
	Ball_Y = Ad_Rect.top + Ball_Normal_Diameter / 2;

	//for (int i = 0; i < Height; i++)
	//	for (int j = 0; j < Width; j++)
	//		Show_Under_Brick(Level_X + j, Level_Y + i);
}
//------------------------------------------------------------------------------------------------------------
void AAdvertisment::Act()
{
	int i, j;
	RECT rect{};

	for (i = 0; i < Height; i++)
		for (j = 0; j < Width; j++)
			if (Bricks_Regions[i * Width + j] != 0)
			{
				rect.left = Ad_Rect.left + j * AsConfig::Cell_Width * AsConfig::Global_Scale;
				rect.top = Ad_Rect.top + i * AsConfig::Cell_Height * AsConfig::Global_Scale;
				rect.right = rect.left + AsConfig::Cell_Width * AsConfig::Global_Scale;
				rect.bottom = rect.top + AsConfig::Cell_Height * AsConfig::Global_Scale;

				AsTools::Invalidate_Rect(rect);
			}

	Falling_Speed += Acceleration_Step;
	Ball_Y_Offset = Max_Ball_Y_Offset - (int)(Falling_Speed * Falling_Speed);

	if (Ball_Y_Offset <= Min_Ball_Y_Offset + Deformation_Height)
		Deformation_Ratio = (double)(Ball_Y_Offset - Min_Ball_Y_Offset) / (double)Deformation_Height;
	else
		Deformation_Ratio = 1.0;

	if (Ball_Y_Offset > Max_Ball_Y_Offset || Ball_Y_Offset < Min_Ball_Y_Offset)
		Acceleration_Step = -Acceleration_Step;
}
//------------------------------------------------------------------------------------------------------------
void AAdvertisment::Clear(HDC hdc, RECT &paint_area)
{
	//!!!
}
//------------------------------------------------------------------------------------------------------------
void AAdvertisment::Draw(HDC hdc, RECT &paint_area)
{
	int i, j;
	const int scale = AsConfig::Global_Scale;
	const double d_scale = AsConfig::D_Global_Scale;
	HRGN region;
	RECT intersection_rect;

	if (! IntersectRect(&intersection_rect, &paint_area, &Ad_Rect) )
		return;

	SelectClipRgn(hdc, Empty_Region);

	for (i = 0; i < Height; i++)
		for (j = 0; j < Width; j++)
		{
			region = Bricks_Regions[i * Width + j];

			if (region != 0)
				ExtSelectClipRgn(hdc, region, RGN_OR);
		}

	// Рамка рекламы
	AsConfig::BG_Color.Select(hdc);
	AsConfig::Blue_Color.Select_Pen(hdc);
	AsTools::Round_Rect(hdc, Ad_Rect);

	// Стол
	POINT table_points[4]
	{
		{ Ad_Rect.left, Ad_Rect.top + 15 * scale }, 
		{ Ad_Rect.left + 15 * scale + 1, Ad_Rect.top + 10 * scale }, 
		{ Ad_Rect.right - scale + 1, Ad_Rect.top + 15 * scale }, 
		{ Ad_Rect.left + 15 * scale + 1, Ad_Rect.top + 20 * scale - 1 }
	};

	AsConfig::White_Color.Select(hdc);
	Polygon(hdc, table_points, 4);

	// Тень под шариком
	double deformation = (1.0 - Deformation_Ratio) * 2.0 * scale;

	double shadow_width = (double)Ball_Width - 3.0 * d_scale + deformation;
	double shadow_height = (double)Ball_Height - 8.0 * d_scale - deformation;

	int shadow_x = (int)( (double)Ball_X - shadow_width / 2.0);
	int shadow_y = (int)( (double)Ball_Y + shadow_height / 2.0 + (double)Ball_Y_Offset / 5.0) + 8 * scale;

	AsConfig::Blue_Color.Select(hdc);
	Ellipse(hdc, shadow_x, shadow_y, shadow_x + (int)shadow_width - 1, shadow_y + (int)shadow_height - 1);

	// Красная рамка стола
	AsConfig::Red_Highlight.Select_Pen(hdc);
	MoveToEx(hdc, Ad_Rect.left + 2 * scale - 1, Ad_Rect.top + 17 * scale - 1, 0);
	LineTo(hdc, Ad_Rect.left + 15 * scale + 1, Ad_Rect.top + 21 * scale);
	LineTo(hdc, Ad_Rect.right - 3 * scale + scale, Ad_Rect.top + 17 * scale - 1);

	// Синяя рамка стола
	AsConfig::Blue_Highlight.Select_Pen(hdc);
	MoveToEx(hdc, Ad_Rect.left, Ad_Rect.top + 15 * scale, 0);
	LineTo(hdc, Ad_Rect.left + 15 * scale + 1, Ad_Rect.top + 10 * scale);
	LineTo(hdc, Ad_Rect.right - scale + 1, Ad_Rect.top + 15 * scale);
	LineTo(hdc, Ad_Rect.left + 15 * scale + 1, Ad_Rect.top + 20 * scale - 1);
	LineTo(hdc, Ad_Rect.left, Ad_Rect.top + 15 * scale);

	// Шарик
	double ball_width = Ball_Width + deformation;
	double ball_height = Ball_Height - deformation;

	int x = (int)( (double)Ball_X - ball_width / 2.0);
	int y = (int)( (double)Ball_Y - ball_height / 2.0 - (double)Ball_Y_Offset);

	AsConfig::Red_Color.Select(hdc);
	Ellipse(hdc, x, y, x + (int)ball_width, y + (int)ball_height);

	// Блик на шарике
	AsConfig::Symbol_Color.Select_Pen(hdc);
	Arc(hdc, x + scale + 1, y + scale + 1, x + Ball_Normal_Diameter - scale, y + Ball_Normal_Diameter - scale, 
		x + 4 * scale, y + scale, x + scale, y + 3 * scale);

	SelectClipRgn(hdc, 0);
}
//------------------------------------------------------------------------------------------------------------
bool AAdvertisment::Is_Finished()
{
	return false;  // Реклама не заканчивается никогда :()
}
//------------------------------------------------------------------------------------------------------------
void AAdvertisment::Show_Under_Brick(int level_x, int level_y)
{
	int x, y;
	int cell_width = AsConfig::Cell_Width * AsConfig::Global_Scale;
	int cell_height = AsConfig::Cell_Height * AsConfig::Global_Scale;
	RECT rect{};

	x = level_x - Level_X;
	y = level_y - Level_Y;

	rect.left = Ad_Rect.left + x * cell_width;
	rect.top = Ad_Rect.top  + y * cell_height;
	rect.right = rect.left + cell_width;
	rect.bottom = rect.top + cell_height;

	Bricks_Regions[y * Width + x] = CreateRectRgnIndirect(&rect);
}
//------------------------------------------------------------------------------------------------------------




// AActive_Brick
//------------------------------------------------------------------------------------------------------------
void AActive_Brick::Get_Level_Pos(int &x_index, int &y_index) const
{
	x_index = Level_Index_X;
	y_index = Level_Index_Y;
}
//------------------------------------------------------------------------------------------------------------
AActive_Brick::~AActive_Brick()
{
}
//------------------------------------------------------------------------------------------------------------
AActive_Brick::AActive_Brick(EBrick_Type brick_type, int brick_y, int brick_x)
	: Brick_Type(brick_type), Brick_Rect{}, Level_Index_X(brick_x), Level_Index_Y(brick_y)
{
	Brick_Rect.left = (AsConfig::Bricks_Area_X_Offset + brick_x * AsConfig::Cell_Width) * AsConfig::Global_Scale;
	Brick_Rect.top = (AsConfig::Bricks_Area_Y_Offset + brick_y * AsConfig::Cell_Height) * AsConfig::Global_Scale;
	Brick_Rect.right = Brick_Rect.left + AsConfig::Brick_Width * AsConfig::Global_Scale;
	Brick_Rect.bottom = Brick_Rect.top + AsConfig::Brick_Height * AsConfig::Global_Scale;
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick::Clear(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if (! IntersectRect(&intersection_rect, &paint_area, &Brick_Rect) )
		return;

	AsTools::Rect(hdc, Brick_Rect, AsConfig::BG_Color);
}
//------------------------------------------------------------------------------------------------------------
double AActive_Brick::Get_Brick_X_Pos(bool is_center) const
{
	double pos;

	pos = (double)(AsConfig::Bricks_Area_X_Offset + Level_Index_X * AsConfig::Cell_Width);

	if (is_center)
		pos += (double)AsConfig::Brick_Width / 2.0;

	return pos;
}
//------------------------------------------------------------------------------------------------------------
double AActive_Brick::Get_Brick_Y_Pos(bool is_center) const
{
	double pos;

	pos = (double)(AsConfig::Bricks_Area_Y_Offset + Level_Index_Y * AsConfig::Cell_Height);

	if (is_center)
		pos += (double)AsConfig::Brick_Height / 2.0;

	return pos;
}
//------------------------------------------------------------------------------------------------------------




// AActive_Brick
AColor AActive_Brick_Fading::Fading_Red_Colors[Max_Fade_Step];
AColor AActive_Brick_Fading::Fading_Blue_Colors[Max_Fade_Step];
//------------------------------------------------------------------------------------------------------------
AActive_Brick_Fading::~AActive_Brick_Fading()
{
}
//------------------------------------------------------------------------------------------------------------
AActive_Brick_Fading::AActive_Brick_Fading(EBrick_Type brick_type, int brick_y, int brick_x)
	: AActive_Brick(brick_type, brick_y, brick_x), Fade_Step(0)
{
	if (! (brick_type == EBrick_Type::Red || brick_type == EBrick_Type::Blue) )
		AsTools::Throw();
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Fading::Act()
{
	if (Fade_Step < Max_Fade_Step - 1)
	{
		++Fade_Step;
		AsTools::Invalidate_Rect(Brick_Rect);
	}
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Fading::Draw(HDC hdc, RECT &paint_area)
{
	const AColor *color = 0;

	switch (Brick_Type)
	{
	case EBrick_Type::None:
		return;

	case EBrick_Type::Red:
		color = &Fading_Red_Colors[Fade_Step];
		break;

	case EBrick_Type::Blue:
		color = &Fading_Blue_Colors[Fade_Step];
		break;

	default:
		return;
	}
	if (color !=0)
		color->Select(hdc);

	AsTools::Round_Rect(hdc, Brick_Rect);
}
//------------------------------------------------------------------------------------------------------------
bool AActive_Brick_Fading::Is_Finished()
{
	if (Fade_Step >= Max_Fade_Step - 1)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Fading::Setup_Colors()
{
	int i;

	for (i = 0; i < Max_Fade_Step; i++)
	{
		AsTools::Get_Fading_Color(AsConfig::Red_Color, i, Fading_Red_Colors[i], Max_Fade_Step);
		AsTools::Get_Fading_Color(AsConfig::Blue_Color, i, Fading_Blue_Colors[i], Max_Fade_Step);
	}
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Fading::Draw_In_Level(HDC hdc, RECT &brick_rect, EBrick_Type brick_type)
{// Отрисовка неактивного кирпича на уровне

	const AColor *color = 0;

	switch (brick_type)
	{
	case EBrick_Type::None:
		color = &AsConfig::BG_Color;
		break;

	case EBrick_Type::Red:
		color = &AsConfig::Red_Color;
		break;

	case EBrick_Type::Blue:
		color = &AsConfig::Blue_Color;
		break;

	default:
		AsTools::Throw();
	}

	if (color != 0)
		color->Select(hdc);

	AsTools::Round_Rect(hdc, brick_rect);
}
//------------------------------------------------------------------------------------------------------------




// AActive_Brick_Unbreakable
//------------------------------------------------------------------------------------------------------------
AActive_Brick_Unbreakable::~AActive_Brick_Unbreakable()
{
	DeleteObject(Highlight_Region);
}
//------------------------------------------------------------------------------------------------------------
AActive_Brick_Unbreakable::AActive_Brick_Unbreakable(int brick_y, int brick_x)
	: AActive_Brick(EBrick_Type::Unbreakable, brick_y, brick_x), Animation_Step(0), Highlight_Region(0)
{
	Highlight_Region = CreateRoundRectRgn(Brick_Rect.left, Brick_Rect.top, Brick_Rect.right + 1, Brick_Rect.bottom + 1, 2 * AsConfig::Global_Scale, 2 * AsConfig::Global_Scale);
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Unbreakable::Act()
{
	if (Animation_Step < Max_Animation_Step)
	{
		++Animation_Step;
		AsTools::Invalidate_Rect(Brick_Rect);
	}
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Unbreakable::Draw(HDC hdc, RECT &paint_area)
{
	int offset;
	int top_x, bottom_x;

	Draw_In_Level(hdc, Brick_Rect);

	offset = Animation_Step * AsConfig::Global_Scale;

	top_x = Brick_Rect.left - 3 * AsConfig::Global_Scale / 2;
	bottom_x = top_x - 7 * AsConfig::Global_Scale;

	SelectClipRgn(hdc, Highlight_Region);

	AsConfig::Red_Highlight.Select_Pen(hdc);

	MoveToEx(hdc, top_x + offset, Brick_Rect.top, 0);
	LineTo(hdc, bottom_x + offset - 1, Brick_Rect.bottom + 1);

	AsConfig::Blue_Highlight.Select_Pen(hdc);

	MoveToEx(hdc, top_x - 3 * AsConfig::Global_Scale + offset + 2, Brick_Rect.top - 1, 0);
	LineTo(hdc, bottom_x - 3 * AsConfig::Global_Scale + offset + 1, Brick_Rect.bottom);

	SelectClipRgn(hdc, 0);
}
//------------------------------------------------------------------------------------------------------------
bool AActive_Brick_Unbreakable::Is_Finished()
{
	if (Animation_Step >= Max_Animation_Step)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Unbreakable::Draw_In_Level(HDC hdc, RECT &brick_rect)
{// Отрисовка неактивного кирпича на уровне

	AsConfig::White_Color.Select(hdc);
	AsTools::Round_Rect(hdc, brick_rect);
}
//------------------------------------------------------------------------------------------------------------




// AActive_Brick_Multihit
//------------------------------------------------------------------------------------------------------------
AActive_Brick_Multihit::~AActive_Brick_Multihit()
{
}
//------------------------------------------------------------------------------------------------------------
AActive_Brick_Multihit::AActive_Brick_Multihit(int brick_y, int brick_x)
	: AActive_Brick(EBrick_Type::Unbreakable, brick_y, brick_x), Can_Dectroy(false), Rotation_Step(0)
{
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Multihit::Act()
{
	if (Rotation_Step < Max_Rotation_Step)
	{
		++Rotation_Step;
		AsTools::Invalidate_Rect(Brick_Rect);
	}
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Multihit::Draw(HDC hdc, RECT &paint_area)
{
	const int scale = AsConfig::Global_Scale;
	const int brick_half_width = AsConfig::Brick_Width * AsConfig::Global_Scale / 2;
	float alpha;
	XFORM xform{}, old_xform;

	if (Rotation_Step >= Max_Rotation_Step)
	{
		Clear(hdc, paint_area);
		Can_Dectroy = true;
		return;
	}

	Clear(hdc, paint_area);

	alpha = 2.0f * (float)M_PI / (float)Max_Rotation_Step * (float)Rotation_Step;

	xform.eM11 = cosf(alpha);
	xform.eM12 = 0.0f;
	xform.eM21 = 0.0f;
	xform.eM22 = 1.0f;
	xform.eDx = (float)Brick_Rect.left + (float)brick_half_width;
	xform.eDy = (float)Brick_Rect.top;
	GetWorldTransform(hdc, &old_xform);
	SetWorldTransform(hdc, &xform);

	// "Сотня"
	AsConfig::Symbol_Color.Select_Pen(hdc);

	MoveToEx(hdc, - brick_half_width + 3 * scale + 1, AsConfig::Brick_Height * scale - 1, 0);
	LineTo(hdc, - brick_half_width + 3 * scale + 1, scale);
	LineTo(hdc, - brick_half_width + scale + 1, 3 * scale);

	Ellipse(hdc, - brick_half_width + 5 * scale + 1, scale, - brick_half_width + 8 * scale + 1, AsConfig::Brick_Height * scale - scale + 1);
	Ellipse(hdc, - brick_half_width + 10 * scale + 1, scale, - brick_half_width + 13 * scale + 1, AsConfig::Brick_Height * scale - scale + 1);

	SetWorldTransform(hdc, &old_xform);
}
//------------------------------------------------------------------------------------------------------------
bool AActive_Brick_Multihit::Is_Finished()
{
	if (Can_Dectroy)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Multihit::Draw_In_Level(HDC hdc, EBrick_Type brick_type, RECT &brick_rect)
{// Отрисовка неактивного кирпича на уровне

	const int scale = AsConfig::Global_Scale;

	AsConfig::White_Color.Select(hdc);
	AsTools::Round_Rect(hdc, brick_rect);

	AsConfig::Red_Color.Select(hdc);	
	Rectangle(hdc, brick_rect.left + scale, brick_rect.top + scale, brick_rect.right - scale - 1, brick_rect.bottom - scale - 1);

	switch (brick_type)
	{
	case EBrick_Type::Multihit_1:
		Draw_Inner_Rects(hdc, brick_rect, 2, 10);
		break;

	case EBrick_Type::Multihit_2:
		Draw_Inner_Rects(hdc, brick_rect, 2, 4);
		Draw_Inner_Rects(hdc, brick_rect, 8, 4);
		break;

	case EBrick_Type::Multihit_3:
		Draw_Inner_Rects(hdc, brick_rect, 2, 2);
		Draw_Inner_Rects(hdc, brick_rect, 6, 2);
		Draw_Inner_Rects(hdc, brick_rect, 10, 2);
		break;

	case EBrick_Type::Multihit_4:
		Draw_Inner_Rects(hdc, brick_rect, 2, 2);
		Draw_Inner_Rects(hdc, brick_rect, 5, 2);
		Draw_Inner_Rects(hdc, brick_rect, 8, 2);
		Draw_Inner_Rects(hdc, brick_rect, 11, 2);
		break;

	default:
		AsTools::Throw();
	}
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Multihit::Draw_Inner_Rects(HDC hdc, RECT &brick_rect, int x, int inner_width)
{// Отрисовка внутренней части неактивного Multihit кирпича на уровне

	const int scale = AsConfig::Global_Scale;
	RECT inner_rect_small{};

	inner_rect_small.left = brick_rect.left + x * scale;
	inner_rect_small.top = brick_rect.top + 2 * scale;
	inner_rect_small.right = inner_rect_small.left + inner_width * scale;
	inner_rect_small.bottom = brick_rect.bottom - 2 * scale;

	AsConfig::BG_Color.Select(hdc);
	Rectangle(hdc, inner_rect_small.left + scale, inner_rect_small.top + scale, inner_rect_small.right + scale - 1, inner_rect_small.bottom + scale - 1);

	AsTools::Rect(hdc, inner_rect_small, AsConfig::Blue_Color);
}
//------------------------------------------------------------------------------------------------------------




// AActive_Brick_Teleport
//------------------------------------------------------------------------------------------------------------
AActive_Brick_Teleport::~AActive_Brick_Teleport()
{
}
//------------------------------------------------------------------------------------------------------------
AActive_Brick_Teleport::AActive_Brick_Teleport(int brick_y, int brick_x, ABall_Object *ball, AActive_Brick_Teleport *destination_teleport)
	: AActive_Brick(EBrick_Type::Teleport, brick_y, brick_x), Release_Direction(EDirection_Type::Up), Teleport_State(ETeleport_State::Starting), Ball(ball), Animation_Step(0), Destination_Teleport(destination_teleport)
{
	//Ball_X = (double)Brick_Rect.left / (double)AsConfig::Global_Scale + (double)AsConfig::Brick_Width / 2.0;
	//Ball_Y = (double)Brick_Rect.top / (double)AsConfig::Global_Scale + (double)AsConfig::Brick_Height / 2.0;  //!!!

	Ball_X = Get_Brick_X_Pos(true);
	Ball_Y = Get_Brick_Y_Pos(true);

	if (Destination_Teleport != 0)
	{
		Ball->Set_State(EBall_State::Teleporting_Stage_1, Ball_X, Ball_Y);
		Destination_Teleport->Ball = Ball;
	}
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Teleport::Act()
{
	//if (AsConfig::Current_Timer_Tick % 10 != 0)
	//	return;		
	double direction;

	switch (Teleport_State)
	{
	case ETeleport_State::Starting:
		if (Animation_Step < Max_Animation_Step)
		{
			++Animation_Step;
			AsTools::Invalidate_Rect(Brick_Rect);
			break;
		}
		else
			Teleport_State = ETeleport_State::Finishing;

		if (Destination_Teleport != 0)
			Ball = 0;
		else
			Ball->Set_State(EBall_State::Teleporting_Stage_2, Ball_X, Ball_Y);
		break;


	case ETeleport_State::Finishing:
		if (Animation_Step > 0)
		{
			--Animation_Step;
			AsTools::Invalidate_Rect(Brick_Rect);
			break;
		}
		else
			Teleport_State = ETeleport_State::Done;

		if (Destination_Teleport == 0)
		{
			switch (Release_Direction)
			{
			case EDirection_Type::Left:
				Ball_X = Get_Brick_X_Pos(false) - AsConfig::Ball_Radius;
				Ball_Y = Get_Brick_Y_Pos(true);
				break;

			case EDirection_Type::Up:
				Ball_X = Get_Brick_X_Pos(true);
				Ball_Y = Get_Brick_Y_Pos(false) - AsConfig::Ball_Radius;
				break;

			case EDirection_Type::Right:
				Ball_X = Get_Brick_X_Pos(false) + AsConfig::Ball_Radius + (double)AsConfig::Brick_Width;
				Ball_Y = Get_Brick_Y_Pos(true);
				break;

			case EDirection_Type::Down:
				Ball_X = Get_Brick_X_Pos(true);
				Ball_Y = Get_Brick_Y_Pos(false) + AsConfig::Ball_Radius + (double)AsConfig::Brick_Height;
				break;

			default:
				AsTools::Throw();
			}

			direction = Ball->Get_Direction();
			Ball->Set_State(EBall_State::Normal, Ball_X, Ball_Y);
			Ball->Set_Direction(direction);

			AsTools::Invalidate_Rect(Brick_Rect);
		}

		break;


	case ETeleport_State::Done:
		return;

	default:
		AsTools::Throw();
	}
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Teleport::Draw(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if (! IntersectRect(&intersection_rect, &paint_area, &Brick_Rect) )
		return;

	Draw_In_Level(hdc, Brick_Rect, Animation_Step);

	if (Ball != 0)
		Ball->Draw_Teleporting(hdc, Animation_Step);
}
//------------------------------------------------------------------------------------------------------------
bool AActive_Brick_Teleport::Is_Finished()
{
	if (Teleport_State == ETeleport_State::Done)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Teleport::Draw_In_Level(HDC hdc, RECT &brick_rect, int step)
{// Отрисовка неактивного кирпича на уровне

	const int scale = AsConfig::Global_Scale;

	AsConfig::Red_Color.Select(hdc);
	AsTools::Round_Rect(hdc, brick_rect);

	AsConfig::Teleport_Color.Select(hdc);
	Ellipse(hdc, brick_rect.left + 3 * scale - 1, brick_rect.top + step + 1, brick_rect.right - 3 * scale, brick_rect.bottom - step - scale + 1);
}
//------------------------------------------------------------------------------------------------------------




// AActive_Brick_Ad
//------------------------------------------------------------------------------------------------------------
AActive_Brick_Ad::~AActive_Brick_Ad()
{
}
//------------------------------------------------------------------------------------------------------------
AActive_Brick_Ad::AActive_Brick_Ad(int brick_y, int brick_x, AAdvertisment *advertisment)
	: AActive_Brick(EBrick_Type::Ad, brick_y, brick_x)
{
	advertisment->Show_Under_Brick(Level_Index_X, Level_Index_Y);
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Ad::Act()
{
	AsTools::Invalidate_Rect(Brick_Rect);
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Ad::Draw(HDC hdc, RECT &paint_area)
{
}
//------------------------------------------------------------------------------------------------------------
bool AActive_Brick_Ad::Is_Finished()
{
	return true;
}
//------------------------------------------------------------------------------------------------------------
void AActive_Brick_Ad::Draw_In_Level(HDC hdc, RECT &brick_rect)
{// Отрисовка неактивного кирпича на уровне

	int i;
	const int scale = AsConfig::Global_Scale;
	int circle_diam = 7;
	int x = brick_rect.left;
	int y = brick_rect.top;

	AsConfig::BG_Color.Select(hdc);
	Rectangle(hdc, brick_rect.left, brick_rect.top, brick_rect.right + AsConfig::Global_Scale - 1, brick_rect.bottom + AsConfig::Global_Scale - 1);

	for (i = 0; i < 2; i++)
	{
		AsConfig::Red_Color.Select(hdc);
		Ellipse(hdc, x, y, x + circle_diam * scale - 1, brick_rect.bottom - 1);

		AsConfig::White_Color.Select_Pen(hdc);
		Arc(hdc, x + scale, y + scale, x + (circle_diam - 1) * scale - 1, y + (circle_diam - 1) * scale - 1,
			x + 2 * scale, y + scale, x + scale, y + 3 * scale);

		x += 8 * scale;
	}
}
//------------------------------------------------------------------------------------------------------------
