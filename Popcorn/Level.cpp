#include "Level.h"

// SPoint
//------------------------------------------------------------------------------------------------------------
APoint::APoint()
	: X(0), Y(0)
{

}
//------------------------------------------------------------------------------------------------------------
APoint::APoint(int x, int y)
	: X(x), Y(y)
{

}
//------------------------------------------------------------------------------------------------------------




// AsLevel
//------------------------------------------------------------------------------------------------------------
AsLevel *AsLevel::Level = 0;
//------------------------------------------------------------------------------------------------------------
AsLevel::~AsLevel()
{
	Cansel_All_Activity();
}
//------------------------------------------------------------------------------------------------------------
AsLevel::AsLevel()
	: Parashute_Brick_Color(AsConfig::Red_Color, AsConfig::Blue_Color, AsConfig::Global_Scale), Advertisment(0), Need_To_Cansel_All(false),
	Current_Brick_Left_X(0.0), Current_Brick_Right_X(0.0), Current_Brick_Top_Y(0.0), Current_Brick_Low_Y(0.0),
	Level_Rect{}, Active_Bricks{}, Falling_Letters{}
{
	Level = this;
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Check_Hit(double next_x_pos, double next_y_pos, ABall_Object *ball)
{
	int i, j;
	double min_ball_x, max_ball_x;
	double min_ball_y, max_ball_y;
	int min_level_x, max_level_x;
	int min_level_y, max_level_y;
	bool got_horizontal_hit, got_vertical_hit;
	double horizontal_reflection_pos, vertical_reflection_pos;

	if (next_y_pos - AsConfig::Ball_Radius > AsConfig::Bricks_Area_Y_Offset + (AsConfig::Level_Height - 1) * AsConfig::Cell_Height + AsConfig::Brick_Height)
		return false;

	min_ball_x = next_x_pos - AsConfig::Ball_Radius;
	max_ball_x = next_x_pos + AsConfig::Ball_Radius;
	min_ball_y = next_y_pos - AsConfig::Ball_Radius;
	max_ball_y = next_y_pos + AsConfig::Ball_Radius;

	min_level_x = (int)( (min_ball_x - AsConfig::Bricks_Area_X_Offset) / (double)AsConfig::Cell_Width);
	max_level_x = (int)( (max_ball_x - AsConfig::Bricks_Area_X_Offset) / (double)AsConfig::Cell_Width);
	min_level_y = (int)( (min_ball_y - AsConfig::Bricks_Area_Y_Offset) / (double)AsConfig::Cell_Height);
	max_level_y = (int)( (max_ball_y - AsConfig::Bricks_Area_Y_Offset) / (double)AsConfig::Cell_Height);

	if (min_level_x < 0)
		min_level_x = 0;

	if (min_level_y < 0)
		min_level_y = 0;

	if (max_level_x >= AsConfig::Level_Width)
		max_level_x = AsConfig::Level_Width - 1;

	if (max_level_y >= AsConfig::Level_Height)
		max_level_y = AsConfig::Level_Height - 1;

	for (i = max_level_y; i >= min_level_y; i--)
	{
		Current_Brick_Top_Y = AsConfig::Bricks_Area_Y_Offset + i * AsConfig::Cell_Height;
		Current_Brick_Low_Y = Current_Brick_Top_Y + AsConfig::Brick_Height;

		for (j = min_level_x; j <= max_level_x; j++)
		{
			if (Current_Level[i][j] == 0)
				continue;

			Current_Brick_Left_X = AsConfig::Bricks_Area_X_Offset + j * AsConfig::Cell_Width;
			Current_Brick_Right_X = Current_Brick_Left_X + AsConfig::Brick_Width;

			got_vertical_hit = Check_Vertical_Hit(next_x_pos, next_y_pos, j, i, vertical_reflection_pos, ball);
			got_horizontal_hit = Check_Horizontal_Hit(next_x_pos, next_y_pos, j, i, horizontal_reflection_pos, ball);

			if (got_vertical_hit && got_horizontal_hit)
			{
				if (On_Hit(i, j, ball, true) )
				{
					if (vertical_reflection_pos < horizontal_reflection_pos)
						ball->Reflect(true);
					else
						ball->Reflect(false);
				}

				return true;
			}
			else if (got_vertical_hit)
			{
				if (On_Hit(i, j, ball, true) )
					ball->Reflect(true);

				return true;
			}
			else if (got_horizontal_hit)
			{
				if (On_Hit(i, j, ball, false) )
					ball->Reflect(false);
				return true;
			}
		}
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Check_Hit(double next_x_pos, double next_y_pos)
{
	int level_index_x, level_index_y;

	level_index_x = (int)( (next_x_pos - AsConfig::Bricks_Area_X_Offset) / (double)AsConfig::Cell_Width);
	level_index_y = (int)( (next_y_pos - AsConfig::Bricks_Area_Y_Offset) / (double)AsConfig::Cell_Height);

	if (level_index_x < 0 || level_index_x >= AsConfig::Level_Width)
		return false;

	if (level_index_y < 0 || level_index_y >= AsConfig::Level_Height)
		return false;

	if (Current_Level[level_index_y][level_index_x] == 0)
		return false;

	On_Hit(level_index_y, level_index_x);

	return true;
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Begin_Movement()
{
	// Заглушка т.к. этот метод не используется
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Finish_Movement()
{
	// Заглушка т.к. этот метод не используется
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Advance(double max_speed)
{
	// Заглушка т.к. этот метод не используется
}
//------------------------------------------------------------------------------------------------------------
double AsLevel::Get_Speed()
{
	return 0.0;  // Заглушка т.к. этот метод не используется
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Act()
{
	Act_Object(Active_Bricks);
	Act_Object(Falling_Letters);

	if (Advertisment != 0)
		Advertisment->Act();
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Clear(HDC hdc, RECT &paint_area)
{
	for (auto *curr_letter : Falling_Letters)
		curr_letter->Clear(hdc, paint_area);

	if (Advertisment != 0)
		Advertisment->Clear(hdc, paint_area);

	if (Need_To_Cansel_All)
	{
		Cansel_All_Activity();
		Need_To_Cansel_All = false;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Draw(HDC hdc, RECT &paint_area)
{
	int i, j;
	RECT intersection_rect, brick_rect{};

	Clear(hdc, paint_area);

	if (Advertisment != 0)
		Advertisment->Draw(hdc, paint_area);

	if (IntersectRect(&intersection_rect, &paint_area, &Level_Rect) )
	{
		for (i = 0; i < AsConfig::Level_Height; i++)
			for (j = 0; j < AsConfig::Level_Width; j++)
			{
				brick_rect.left = (AsConfig::Bricks_Area_X_Offset + AsConfig::Cell_Width * j) * AsConfig::Global_Scale;
				brick_rect.top = (AsConfig::Bricks_Area_Y_Offset + AsConfig::Cell_Height * i) * AsConfig::Global_Scale;
				brick_rect.right = brick_rect.left + AsConfig::Brick_Width * AsConfig::Global_Scale;
				brick_rect.bottom = brick_rect.top + AsConfig::Brick_Height * AsConfig::Global_Scale;

				if (IntersectRect(&intersection_rect, &paint_area, &brick_rect))
					Draw_Brick(hdc, brick_rect, j, i);

			}

		for (auto *curr_object: Active_Bricks)
			curr_object->Draw(hdc, paint_area);
	}

	for (auto *curr_object : Falling_Letters)
		curr_object->Draw(hdc, paint_area);
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Is_Finished()
{
	return false;  // Заглушка. Этот метод не используется
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Init()
{
	int i;
	ALevel_Data *level_data;

	Level_Rect.left = AsConfig::Bricks_Area_X_Offset * AsConfig::Global_Scale;
	Level_Rect.top = AsConfig::Bricks_Area_Y_Offset * AsConfig::Global_Scale;
	Level_Rect.right = Level_Rect.left + AsConfig::Cell_Width * AsConfig::Level_Width * AsConfig::Global_Scale;
	Level_Rect.bottom = Level_Rect.top + AsConfig::Cell_Height * AsConfig::Level_Height * AsConfig::Global_Scale;

	memset(Current_Level, 0, sizeof(Current_Level) );

	for (i = 0; i < ALevel_Data::Max_Level_Number; i++)
	{
		level_data = new ALevel_Data(i + 1);
		Levels_Data.push_back(level_data);
	}
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Set_Current_Level(int level_number)
{
	int i, j;
	EBrick_Type brick_type;
	ALevel_Data *level_data;

	if (level_number < 1 || level_number > (int)Levels_Data.size() )
		AsTools::Throw();

	level_data = Levels_Data[level_number - 1];

	memcpy(Current_Level, level_data->Level, sizeof(Current_Level) );

	Teleport_Bricks_Pos.erase(Teleport_Bricks_Pos.begin(), Teleport_Bricks_Pos.end() );

	for (i = 0; i < AsConfig::Level_Height; i++)
	{
		for (j = 0; j < AsConfig::Level_Width; j++)
		{
			brick_type = (EBrick_Type)Current_Level[i][j];

			if (brick_type == EBrick_Type::Teleport)
				Teleport_Bricks_Pos.emplace_back(j, i);
		}
	}

	if (Teleport_Bricks_Pos.size() == 1)
		AsTools::Throw();  // Телепортов должно быть 0 либо больше 1!

	//Advertisment = new AAdvertisment(6, 9, 2, 3); //!!!
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Get_Falling_Letter(int &index, AFalling_Letter **falling_letter) const
{
	if (index < (int)Falling_Letters.size() )
	{
		*falling_letter = (AFalling_Letter *)Falling_Letters[index++];
		return true;
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Stop()
{
	Need_To_Cansel_All = true;
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Has_Brick_At(int level_x, int level_y)
{
	EBrick_Type brick_type;

	if (level_x < 0 || level_x > AsConfig::Level_Width - 1)
		return false;

	if (level_y < 0 || level_y > AsConfig::Level_Height - 1)
		return false;

	brick_type = (EBrick_Type)Level->Current_Level[level_y][level_x];

	if (brick_type == EBrick_Type::None)
		return false;
	else
		return true;
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Has_Brick_At(RECT &rect)
{
	int i, j;
	int level_x_offset = AsConfig::Bricks_Area_X_Offset * AsConfig::Global_Scale;
	int level_y_offset = AsConfig::Bricks_Area_Y_Offset * AsConfig::Global_Scale;
	int cell_width = AsConfig::Cell_Width * AsConfig::Global_Scale;
	int cell_height = AsConfig::Cell_Height * AsConfig::Global_Scale;
	int min_level_x, max_level_x;
	int min_level_y, max_level_y;

	min_level_x = (int)( (double)(rect.left - level_x_offset) / (double)cell_width);
	max_level_x = (int)( (double)(rect.right - level_x_offset) / (double)cell_width);
	min_level_y = (int)( (double)(rect.top - level_y_offset) / (double)cell_height);
	max_level_y = (int)( (double)(rect.bottom - level_y_offset) / (double)cell_height);


	for (i = min_level_y; i <= max_level_y; i++)
		for (j = min_level_x; j <= max_level_x; j++)
			if (Has_Brick_At(j, i) )
				return true;

	return false;
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::On_Hit(int brick_y, int brick_x, ABall_Object *ball, bool vertical_hit)
{
	bool can_reflect = true;
	EBrick_Type brick_type;

	brick_type = (EBrick_Type)Current_Level[brick_y][brick_x];

	if (brick_type == EBrick_Type::Parashute)
	{
		ball->Set_On_Parachute(brick_y, brick_x);
		Current_Level[brick_y][brick_x] = (char)EBrick_Type::None;
		can_reflect = false;
	}
	else if (Add_Falling_Letter(brick_y, brick_x, brick_type) )
		Current_Level[brick_y][brick_x] = (char)EBrick_Type::None;
	else
		can_reflect = Create_Active_Brick(brick_y, brick_x, brick_type, ball, vertical_hit);

	Redraw_Brick(brick_y, brick_x);

	AsInfo_Panel::Update_Score(EScore_Event_Type::Hit_Brick);

	return can_reflect;
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::On_Hit(int level_y, int level_x)
{
	EBrick_Type brick_type;

	brick_type = (EBrick_Type)Current_Level[level_y][level_x];

	if (brick_type == EBrick_Type::Parashute)
		return;
	else if (Add_Falling_Letter(level_y, level_x, brick_type) )
		Current_Level[level_y][level_x] = (char)EBrick_Type::None;
	else
		Create_Active_Brick(level_y, level_x, brick_type, 0, 0);

	Redraw_Brick(level_y, level_x);

	AsInfo_Panel::Update_Score(EScore_Event_Type::Hit_Brick);
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Create_Active_Brick(int brick_y, int brick_x, EBrick_Type brick_type, ABall_Object *ball, bool vertical_hit)
{
	AActive_Brick *active_brick = 0;

	if (Active_Bricks.size() >= AsConfig::Max_Active_Bricks_Count)
		return true;

	switch (brick_type)
	{
	case EBrick_Type::None:
		return false;

	case EBrick_Type::Invisible:
		return true;

	case EBrick_Type::Red:
	case EBrick_Type::Blue:
		active_brick = new AActive_Brick_Fading(brick_type, brick_y, brick_x);
		Current_Level[brick_y][brick_x] = (char)EBrick_Type::None;
		break;

	case EBrick_Type::Unbreakable:
		active_brick = new AActive_Brick_Unbreakable(brick_y, brick_x);
		break;

	case EBrick_Type::Multihit_1:
		active_brick = new AActive_Brick_Multihit(brick_y, brick_x);
		Current_Level[brick_y][brick_x] = (char)EBrick_Type::None;
		break;

	case EBrick_Type::Multihit_2:
	case EBrick_Type::Multihit_3:
	case EBrick_Type::Multihit_4:
		Current_Level[brick_y][brick_x] = (int)brick_type - 1;
		return true;

	case EBrick_Type::Teleport:
		if (ball != 0)
			Add_Active_Brick_Teleport(brick_x, brick_y, ball, vertical_hit);
		return false;

	case EBrick_Type::Ad:
		active_brick = new AActive_Brick_Ad(brick_y, brick_x, Advertisment);
		Current_Level[brick_y][brick_x] = (char)EBrick_Type::Invisible;
		break;

	default:
		AsTools::Throw();
	}

	if (active_brick != 0)
		Active_Bricks.push_back(active_brick);

	return true;
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Add_Active_Brick_Teleport(int source_x, int source_y, ABall_Object *ball, bool vertical_hit)
{
	int i;
	int dest_x, dest_y;
	double prev_x_pos, prev_y_pos;
	double post_x_pos, post_y_pos;
	bool got_direction = false;
	EDirection_Type direction;
	AActive_Brick_Teleport *sourse_teleport, *destination_teleport;

	ball->Get_Center(prev_x_pos, prev_y_pos);

	destination_teleport = Select_Destination_Teleport(source_y, source_x);
	sourse_teleport = new AActive_Brick_Teleport(source_y, source_x, ball, destination_teleport);

	destination_teleport->Get_Level_Pos(dest_x, dest_y);
	ball->Get_Center(post_x_pos, post_y_pos);

	if (vertical_hit)
	{
		if (prev_y_pos < post_y_pos)
			direction = EDirection_Type::Down;
		else
			direction = EDirection_Type::Up;
	}
	else
	{
		if (prev_x_pos < post_x_pos)
			direction = EDirection_Type::Right;
		else
			direction = EDirection_Type::Left;
	}

	for (i = 0; i < 4; i++)
	{
		switch (direction)
		{
		case EDirection_Type::Left:
			if (dest_x > 0 && Current_Level[dest_y][dest_x - 1] == 0)
				got_direction = true;
			break;

		case EDirection_Type::Right:
			if (dest_x < AsConfig::Level_Width - 1 && Current_Level[dest_y][dest_x + 1] == 0)
				got_direction = true;
			break;

		case EDirection_Type::Up:
			if (dest_y > 0 && Current_Level[dest_y - 1][dest_x] == 0)
				got_direction = true;
			break;

		case EDirection_Type::Down:
			if (dest_y < AsConfig::Level_Height - 1 && Current_Level[dest_y + 1][dest_x] == 0)
				got_direction = true;
			break;

		default:
			AsTools::Throw();
		}

		if (got_direction)
			break;

		direction = (EDirection_Type)( (int)direction - 1);
		ball->Set_Direction(ball->Get_Direction() + M_PI_2);

		if ( (int)direction < 0)
			direction = EDirection_Type::Down;
	}

	destination_teleport->Release_Direction = direction;

	Active_Bricks.push_back(sourse_teleport);
	Active_Bricks.push_back(destination_teleport);
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Add_Falling_Letter(int brick_y, int brick_x, EBrick_Type brick_type)
{
	AFalling_Letter *falling_letter;
	ELetter_Type letter_type;
	int letter_x, letter_y;

	if ( ! (brick_type == EBrick_Type::Red || brick_type == EBrick_Type::Blue) )
		return false;

	if (AsTools::Rand(AsConfig::Hits_Per_Letter) != 0)
		return false;

	if (Falling_Letters.size() > AsConfig::Max_Falling_Letters_count)
		return false;
	
	letter_x = brick_x * AsConfig::Cell_Width + AsConfig::Bricks_Area_X_Offset;
	letter_y = brick_y * AsConfig::Cell_Height + AsConfig::Bricks_Area_Y_Offset;

	letter_type = AFalling_Letter::Get_Random_Letter_Type();

	switch (AsTools::Rand(4))
	{
	case 0:
		letter_type = ELetter_Type::C;
		break;
	case 1:
		letter_type = ELetter_Type::L;
		break;
	case 2:
		letter_type = ELetter_Type::W;
		break;
	case 3:
		letter_type = ELetter_Type::G;
		break;
	}

	falling_letter = new AFalling_Letter(brick_type, letter_type, letter_x, letter_y);
			
	Falling_Letters.push_back( (AGraphics_Object *)falling_letter);

	return true;
}
//------------------------------------------------------------------------------------------------------------
AActive_Brick_Teleport *AsLevel::Select_Destination_Teleport(int source_y, int source_x)
{
	int dest_index;

	if (Teleport_Bricks_Pos.size() < 2)
	{
		AsTools::Throw();
		return 0;
	}

	dest_index = AsTools::Rand(Teleport_Bricks_Pos.size() );

	if (Teleport_Bricks_Pos[dest_index].Y == source_y && Teleport_Bricks_Pos[dest_index].X == source_x)
	{
		++dest_index;

		if (dest_index >= (int)Teleport_Bricks_Pos.size() )
			dest_index = 0;
	}


	return new AActive_Brick_Teleport(Teleport_Bricks_Pos[dest_index].Y, Teleport_Bricks_Pos[dest_index].X, 0, 0);
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Check_Vertical_Hit(double next_x_pos, double next_y_pos, int level_x, int level_y, double &reflection_pos, ABall_Object *ball)
{
	double direction = ball->Get_Direction();

	if (ball->Is_Moving_Up())
	{
		if (Hit_Circle_On_Line(next_y_pos - Current_Brick_Low_Y, next_x_pos, Current_Brick_Left_X, Current_Brick_Right_X, AsConfig::Ball_Radius, reflection_pos))
		{
			if (level_y < AsConfig::Level_Height - 1 && Current_Level[level_y + 1][level_x] == 0 /*&& level_x != AsConfig::Level_Width*/ ||
				level_y == AsConfig::Level_Height - 1)
				return true;
			else
				return false;
		}
	}
	else
	{
		if (Hit_Circle_On_Line(next_y_pos - Current_Brick_Top_Y, next_x_pos, Current_Brick_Left_X, Current_Brick_Right_X, AsConfig::Ball_Radius, reflection_pos) )
		{
			if (level_y > 0 && level_y < AsConfig::Level_Height && Current_Level[level_y - 1][level_x] == 0)
				return true;
			else
				return false;
		}
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Check_Horizontal_Hit(double next_x_pos, double next_y_pos, int level_x, int level_y, double &reflection_pos, ABall_Object *ball)
{
	double direction = ball->Get_Direction();

	if (ball->Is_Moving_Left())
	{
		if (Hit_Circle_On_Line(next_x_pos - Current_Brick_Right_X, next_y_pos, Current_Brick_Top_Y, Current_Brick_Low_Y, AsConfig::Ball_Radius, reflection_pos))
		{
			if (level_x > 0 && level_x < AsConfig::Level_Width - 1 && Current_Level[level_y][level_x + 1] == 0/* && level_x != 0*/)
				return true;
			else
				return false;
		}
	}
	else
	{
		if (Hit_Circle_On_Line(Current_Brick_Left_X - next_x_pos, next_y_pos, Current_Brick_Top_Y, Current_Brick_Low_Y, AsConfig::Ball_Radius, reflection_pos) )
		{
			if (level_x > 0 && level_x < AsConfig::Level_Width && Current_Level[level_y][level_x - 1] == 0)
				return true;
			else
				return false;
		}
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Draw_Brick(HDC hdc, RECT &brick_rect,int level_x, int level_y)
{
	EBrick_Type brick_type = (EBrick_Type)Current_Level[level_y][level_x];

	switch (brick_type)
	{
	case EBrick_Type::None:
		break;

	case EBrick_Type::Red:
	case EBrick_Type::Blue:
		AActive_Brick_Fading::Draw_In_Level(hdc, brick_rect, brick_type);
		break;

	case EBrick_Type::Unbreakable:
		AActive_Brick_Unbreakable::Draw_In_Level(hdc, brick_rect);
		break;

	case EBrick_Type::Multihit_1:
	case EBrick_Type::Multihit_2:
	case EBrick_Type::Multihit_3:
	case EBrick_Type::Multihit_4:
		AActive_Brick_Multihit::Draw_In_Level(hdc, brick_type, brick_rect);
		break;

	case EBrick_Type::Parashute:
		Draw_Parashute_Brick_In_Level(hdc, brick_rect);
		break;

	case EBrick_Type::Teleport:
		AActive_Brick_Teleport::Draw_In_Level(hdc, brick_rect);
		break;

	case EBrick_Type::Ad:
		AActive_Brick_Ad::Draw_In_Level(hdc, brick_rect);
		break;

	case EBrick_Type::Invisible:
		break;

	default:
		AsTools::Throw();
	}
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Draw_Parashute_Brick_In_Level(HDC hdc, RECT &brick_rect)
{
	Draw_Parashute_Segment(hdc, brick_rect, 0, 4);
	Draw_Parashute_Segment(hdc, brick_rect, 4, 6);
	Draw_Parashute_Segment(hdc, brick_rect, 10, 4);
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Draw_Parashute_Segment(HDC hdc, RECT &brick_rect, int x_offset, int segment_width)
{
	int scale = AsConfig::Global_Scale;
	RECT segment_rect{};

	Parashute_Brick_Color.Select(hdc);

	segment_rect.left = brick_rect.left + x_offset * scale + 1;
	segment_rect.top = brick_rect.top + 1;
	segment_rect.right = segment_rect.left + segment_width * scale + 1;
	segment_rect.bottom = segment_rect.top + 3 * scale;

	AsTools::Round_Rect(hdc, segment_rect);

	segment_rect.top += 3 * scale + 1;
	segment_rect.bottom = segment_rect.top + 3 * scale;

	AsTools::Round_Rect(hdc, segment_rect);
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Redraw_Brick(int brick_y, int brick_x)
{
	RECT brick_rect{};

	brick_rect.left = (AsConfig::Bricks_Area_X_Offset + brick_x * AsConfig::Cell_Width) * AsConfig::Global_Scale;
	brick_rect.top = (AsConfig::Bricks_Area_Y_Offset + brick_y * AsConfig::Cell_Height) * AsConfig::Global_Scale;
	brick_rect.right = brick_rect.left + AsConfig::Brick_Width * AsConfig::Global_Scale;
	brick_rect.bottom = brick_rect.top + AsConfig::Brick_Height * AsConfig::Global_Scale;

	AsTools::Invalidate_Rect(brick_rect);
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Act_Object(std::vector<AGraphics_Object *> &graphics_object)
{
	auto it = graphics_object.begin();

	while (it != graphics_object.end() )
	{
		(*it)->Act();

		if ( (*it)->Is_Finished() )
		{
			delete *it;
			it = graphics_object.erase(it);
		}
		else
			it++;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Delete_Object(std::vector<AGraphics_Object *> &graphics_object)
{
	for (auto it = graphics_object.begin(); it != graphics_object.end(); it++)
		delete *it;

	graphics_object.erase(graphics_object.begin(), graphics_object.end() );
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Cansel_All_Activity()
{	
	Delete_Object(Active_Bricks);
	Delete_Object(Falling_Letters);
}
//------------------------------------------------------------------------------------------------------------
