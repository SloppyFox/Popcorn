﻿#include "Level.h"

// APoint
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
	Cancel_All_Activity();
}
//------------------------------------------------------------------------------------------------------------
AsLevel::AsLevel()
: Level_Rect{}, Need_To_Cancel_All(false), Parachute_Color(AsConfig::Red_Color, AsConfig::Blue_Color, AsConfig::Global_Scale),
  Advertisement(0)
{
	Level = this;
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Check_Hit(double next_x_pos, double next_y_pos, ABall_Object *ball)
{// Корректируем позицию при отражении от кирпичей

	int i, j;
	double direction;
	double min_ball_x, max_ball_x;
	double min_ball_y, max_ball_y;
	int min_level_x, max_level_x;
	int min_level_y, max_level_y;
	bool got_horizontal_hit, got_vertical_hit;
	double horizontal_reflection_pos, vertical_reflection_pos;

	if (next_y_pos + AsConfig::Ball_Radius > AsConfig::Level_Y_Offset + (AsConfig::Level_Height - 1) * AsConfig::Cell_Height + AsConfig::Brick_Height)
		return false;

	direction = ball->Get_Direction();

	min_ball_x = next_x_pos - AsConfig::Ball_Radius;
	max_ball_x = next_x_pos + AsConfig::Ball_Radius;
	min_ball_y = next_y_pos - AsConfig::Ball_Radius;
	max_ball_y = next_y_pos + AsConfig::Ball_Radius;

	min_level_x = (int)( (min_ball_x - AsConfig::Level_X_Offset) / (double)AsConfig::Cell_Width);
	if (min_level_x < 0)
		min_level_x = 0;

	max_level_x = (int)( (max_ball_x - AsConfig::Level_X_Offset) / (double)AsConfig::Cell_Width);
	if (max_level_x >= AsConfig::Level_Width - 1)
		max_level_x = AsConfig::Level_Width - 1;

	min_level_y = (int)( (min_ball_y - AsConfig::Level_Y_Offset) / (double)AsConfig::Cell_Height);
	if (min_level_y < 0)
		min_level_y = 0;

	max_level_y = (int)( (max_ball_y - AsConfig::Level_Y_Offset) / (double)AsConfig::Cell_Height);
	if (max_level_y >= AsConfig::Level_Height - 1)
		max_level_y = AsConfig::Level_Height - 1;

	for (i = max_level_y; i >= min_level_y; i--)
	{
		Current_Brick_Top_Y = AsConfig::Level_Y_Offset + i * AsConfig::Cell_Height;
		Current_Brick_Low_Y = Current_Brick_Top_Y + AsConfig::Brick_Height;

		for (j = min_level_x; j <= max_level_x; j++)
		{
			if (Current_Level[i][j] == 0)
				continue;

			Current_Brick_Left_X = AsConfig::Level_X_Offset + j * AsConfig::Cell_Width;
			Current_Brick_Right_X = Current_Brick_Left_X + AsConfig::Brick_Width;

			got_horizontal_hit = Check_Horizontal_Hit(next_x_pos, next_y_pos, j, i, ball, horizontal_reflection_pos);

			got_vertical_hit = Check_Vertical_Hit(next_x_pos, next_y_pos, j, i, ball, vertical_reflection_pos);

			if (got_horizontal_hit && got_vertical_hit)
			{
				if (On_Hit(j, i, ball, true) )
				{
					if (vertical_reflection_pos < horizontal_reflection_pos)
						ball->Reflect(true);
					else
						ball->Reflect(false);
				}

				return true;
			}
			else
				if (got_horizontal_hit)
				{
					if (On_Hit(j, i, ball, false) )
						ball->Reflect(false);

					return true;
				}
				else
					if (got_vertical_hit)
					{
						if (On_Hit(j, i, ball, true) )
							ball->Reflect(true);

						return true;
					}
		}
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Check_Hit(double next_x_pos, double next_y_pos)
{// Возврат: true, если в позиции (next_x_pos, next_y_pos) луч коснётся кирпича

	int level_x_index, level_y_index;

	level_x_index = (int)( (next_x_pos - AsConfig::Level_X_Offset) / (double)AsConfig::Cell_Width);
	level_y_index = (int)( (next_y_pos - AsConfig::Level_Y_Offset) / (double)AsConfig::Cell_Height);

	if (level_x_index < 0 || level_x_index >= AsConfig::Level_Width)
		return false;

	if (level_y_index < 0 || level_y_index >= AsConfig::Level_Height)
		return false;

	if (Current_Level[level_y_index][level_x_index] == 0)
		return false;

	On_Hit(level_x_index, level_y_index, 0, true);

	return true;
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Begin_Movement()
{
	// Заглушка, пока не используется
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Finish_Movement()
{
	// Заглушка, пока не используется
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Advance(double max_speed)
{
	// Заглушка, пока не используется
}
//------------------------------------------------------------------------------------------------------------
double AsLevel::Get_Speed()
{
	return 0.0;  // Заглушка, пока не используется
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Act()
{
	Act_Objects(Active_Bricks);
	Act_Objects(Falling_Letters);

	if (Advertisement != 0)
		Advertisement->Act();
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Clear(HDC hdc, RECT &paint_area)
{// Стираем движущиеся объекты

	for (auto *letter : Falling_Letters)
		letter->Clear(hdc, paint_area);

	if (Advertisement != 0)
		Advertisement->Clear(hdc, paint_area);

	if (Need_To_Cancel_All)
	{
		Cancel_All_Activity();
		Need_To_Cancel_All = false;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Draw(HDC hdc, RECT &paint_area)
{// Вывод всех объектов уровня

	int i, j;
	RECT intersection_rect, brick_rect;

	if (Advertisement != 0)
		Advertisement->Draw(hdc, paint_area);

	if (IntersectRect(&intersection_rect, &paint_area, &Level_Rect) )
	{
		for (i = 0; i < AsConfig::Level_Height; i++)
			for (j = 0; j < AsConfig::Level_Width; j++)
			{
				brick_rect.left = (AsConfig::Level_X_Offset + j * AsConfig::Cell_Width) * AsConfig::Global_Scale;
				brick_rect.top = (AsConfig::Level_Y_Offset + i * AsConfig::Cell_Height) * AsConfig::Global_Scale;
				brick_rect.right = brick_rect.left + AsConfig::Brick_Width * AsConfig::Global_Scale;
				brick_rect.bottom = brick_rect.top + AsConfig::Brick_Height * AsConfig::Global_Scale;

				if (IntersectRect(&intersection_rect, &paint_area, &brick_rect) )
					Draw_Brick(hdc, brick_rect, j, i);
			}

		for (auto *brick : Active_Bricks)
			brick->Draw(hdc, paint_area);
	}

	for (auto *letter : Falling_Letters)
		letter->Draw(hdc, paint_area);
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Is_Finished()
{
	return false;  // Заглушка, т.к. этот метод не используется
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Init()
{
	int i;
	ALevel_Data *level_data;

	Level_Rect.left = AsConfig::Level_X_Offset * AsConfig::Global_Scale;
	Level_Rect.top = AsConfig::Level_Y_Offset * AsConfig::Global_Scale;
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
		AsConfig::Throw();

	level_data = Levels_Data[level_number - 1];

	memcpy(Current_Level, level_data->Level, sizeof(Current_Level) );

	// 1. Считаем телепорты
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
		AsConfig::Throw();  // Телепортов должно быть 0 либо больше 1!

	Advertisement = new AAdvertisement(9, 6, 2, 3);
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Get_Next_Falling_Letter(int &index, AFalling_Letter **falling_letter)
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
	Need_To_Cancel_All = true;
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Has_Brick_At(int level_x, int level_y)
{
	EBrick_Type brick_type;

	if (level_x < 0 || level_x >= AsConfig::Level_Width)
		return false;

	if (level_y < 0 || level_y >= AsConfig::Level_Height)
		return false;

	brick_type = (EBrick_Type)Level->Current_Level[level_y][level_x];

	if (brick_type == EBrick_Type::None)
		return false;
	else
		return true;
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Has_Brick_At(RECT &monster_rect)
{
	int i, j;
	int level_x_offs = AsConfig::Level_X_Offset * AsConfig::Global_Scale;
	int level_y_offs = AsConfig::Level_Y_Offset * AsConfig::Global_Scale;
	int x_step = AsConfig::Cell_Width * AsConfig::Global_Scale;
	int y_step = AsConfig::Cell_Height * AsConfig::Global_Scale;
	int min_level_x, max_level_x;
	int min_level_y, max_level_y;
	int min_cell_x, min_cell_y;

	min_level_x = (monster_rect.left - level_x_offs) / x_step;
	max_level_x = (monster_rect.right - level_x_offs) / x_step;

	min_level_y = (monster_rect.top - level_y_offs) / y_step;
	max_level_y = (monster_rect.bottom - level_y_offs) / y_step;

	// Т.к. ячейка уровня больше кипича (хотя и начинается в одинаковых с кирпичом координатах),
	// то она имеет правую и нижнюю пустую полосу, в которой может находиться монстр.
	// Игнорируем ряд (или столбец) кирпичей, если монстр попал в ячейку но не попал в кирпич.

	min_cell_x = min_level_x * y_step + level_x_offs;
	min_cell_y = min_level_y * y_step + level_y_offs;

	if (monster_rect.left - min_cell_x > AsConfig::Brick_Width * AsConfig::Global_Scale)
		++min_level_x;

	if (monster_rect.top - min_cell_y > AsConfig::Brick_Height * AsConfig::Global_Scale)
		++min_level_y;

	if (min_level_x >= AsConfig::Level_Width)
		min_level_x = AsConfig::Level_Width - 1;

	if (max_level_x >= AsConfig::Level_Width)
		max_level_x = AsConfig::Level_Width - 1;

	if (min_level_y >= AsConfig::Level_Height)
		min_level_y = AsConfig::Level_Height - 1;

	if (max_level_y >= AsConfig::Level_Height)
		max_level_y = AsConfig::Level_Height - 1;

	for (i = min_level_y; i <= max_level_y; i++)
		for (j = min_level_x; j <= max_level_x; j++)
			if (Has_Brick_At(j, i) )
				return true;

	return false;
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::On_Hit(int brick_x, int brick_y, ABall_Object *ball, bool vertical_hit)
{
	EBrick_Type brick_type;
	bool can_reflect = true;

	brick_type = (EBrick_Type)Current_Level[brick_y][brick_x];

	if (ball == 0 && brick_type == EBrick_Type::Parachute)
	{
		brick_type = EBrick_Type::Red;
		Current_Level[brick_y][brick_x] = (char)brick_type;
	}

	if (brick_type == EBrick_Type::Parachute)
	{
		ball->Set_On_Parachute(brick_x, brick_y);
		Current_Level[brick_y][brick_x] = (char)EBrick_Type::None;
	}
	else if (Add_Falling_Letter(brick_x, brick_y, brick_type) )
		Current_Level[brick_y][brick_x] = (char)EBrick_Type::None;
	else
		can_reflect = Create_Active_Brick(brick_x, brick_y, brick_type, ball, vertical_hit);

	Redraw_Brick(brick_x, brick_y);

	AsInfo_Panel::Update_Score(EScore_Event_Type::Hit_Brick);

	return can_reflect;
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Redraw_Brick(int brick_x, int brick_y)
{
	RECT brick_rect;

	brick_rect.left = (AsConfig::Level_X_Offset + brick_x * AsConfig::Cell_Width) * AsConfig::Global_Scale;
	brick_rect.top = (AsConfig::Level_Y_Offset + brick_y * AsConfig::Cell_Height) * AsConfig::Global_Scale;
	brick_rect.right = brick_rect.left + AsConfig::Brick_Width * AsConfig::Global_Scale;
	brick_rect.bottom = brick_rect.top + AsConfig::Brick_Height * AsConfig::Global_Scale;

	AsTools::Invalidate_Rect(brick_rect);
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Add_Falling_Letter(int brick_x, int brick_y, EBrick_Type brick_type)
{// Создаём падающую букву, если можем

	int letter_x, letter_y;
	ELetter_Type letter_type;
	AFalling_Letter *falling_letter;

	if (! (brick_type == EBrick_Type::Red || brick_type == EBrick_Type::Blue) )
		return false;

	if (AsTools::Rand(AsConfig::Hits_Per_Letter) != 0)
		return false;

	if (Falling_Letters.size() >= AsConfig::Max_Falling_Letters_Count)
		return false;

	letter_x = (brick_x * AsConfig::Cell_Width + AsConfig::Level_X_Offset) * AsConfig::Global_Scale;
	letter_y = (brick_y * AsConfig::Cell_Height + AsConfig::Level_Y_Offset) * AsConfig::Global_Scale;

	//letter_type = AFalling_Letter::Get_Random_Letter_Type();
	switch (AsTools::Rand(4) )
	{
	case 0:
		letter_type = ELetter_Type::O;
		break;

	case 1:
		letter_type = ELetter_Type::L;
		break;

	case 2:
		letter_type = ELetter_Type::K;
		break;

	case 3:
		letter_type = ELetter_Type::W;
		break;
	}
	//letter_type = ELetter_Type::G;

	falling_letter = new AFalling_Letter(brick_type, letter_type, letter_x, letter_y);
	Falling_Letters.push_back(falling_letter);

	return true;
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Create_Active_Brick(int brick_x, int brick_y, EBrick_Type brick_type, ABall_Object *ball, bool vertical_hit)
{// Создаём активный кирпич, если можем

	AActive_Brick *active_brick = 0;

	if (Active_Bricks.size() >= AsConfig::Max_Active_Bricks_Count)
		return true;  // Активных кирпичей слишком много!

	switch (brick_type)
	{
	case EBrick_Type::None:
		return true;

	case EBrick_Type::Red:
	case EBrick_Type::Blue:
		active_brick = new AActive_Brick_Red_Blue(brick_type, brick_x, brick_y);
		Current_Level[brick_y][brick_x] = (char)EBrick_Type::None;
		break;

	case EBrick_Type::Unbreakable:
		active_brick = new AActive_Brick_Unbreakable(brick_x, brick_y);
		break;

	case EBrick_Type::Multihit_1:
		active_brick = new AActive_Brick_Multihit(brick_x, brick_y);
		Current_Level[brick_y][brick_x] = (char)EBrick_Type::None;
		break;

	case EBrick_Type::Multihit_2:
	case EBrick_Type::Multihit_3:
	case EBrick_Type::Multihit_4:
		Current_Level[brick_y][brick_x] = (char)brick_type - 1;
		break;

	case EBrick_Type::Parachute:
		AsConfig::Throw();  // Для парашюта активный кирпич не создаётся!
		break;

	case EBrick_Type::Teleport:
		if (ball != 0)
			Add_Active_Brick_Teleport(brick_x, brick_y, ball, vertical_hit);
		return false;

	case EBrick_Type::Ad:
		active_brick = new AActive_Brick_Ad(brick_x, brick_y, Advertisement);
		Current_Level[brick_y][brick_x] = (char)EBrick_Type::Invisible;
		break;

	case EBrick_Type::Invisible:
		return true;

	default:
		AsConfig::Throw();
	}

	if (active_brick != 0)
		Active_Bricks.push_back(active_brick);

	return true;
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Add_Active_Brick_Teleport(int brick_x, int brick_y, ABall_Object *ball, bool vertical_hit)
{
	int i;
	bool got_direction;
	EDirection_Type direction;
	int dest_brick_x, dest_brick_y;
	double pre_teleport_x_pos, pre_teleport_y_pos;
	double curr_ball_x_pos, curr_ball_y_pos;
	AActive_Brick_Teleport *source_teleport, *destination_teleport;

	ball->Get_Center(pre_teleport_x_pos, pre_teleport_y_pos);  // Позиция мячика перед входом в телепорт

	destination_teleport = Select_Destination_Teleport(brick_x, brick_y);
	source_teleport = new AActive_Brick_Teleport(brick_x, brick_y, ball, destination_teleport);

	destination_teleport->Get_Level_Pos(dest_brick_x, dest_brick_y);

	// После создания телепорта мячик стал в его центре
	ball->Get_Center(curr_ball_x_pos, curr_ball_y_pos);

	// Определяем направление, откуда прилетел мячик
	if (vertical_hit)
	{
		if (pre_teleport_y_pos < curr_ball_y_pos)
			direction = EDirection_Type::Down;
		else
			direction = EDirection_Type::Up;
	}
	else
	{
		if (pre_teleport_x_pos < curr_ball_x_pos)
			direction = EDirection_Type::Right;
		else
			direction = EDirection_Type::Left;
	}

	// Перебираем все направления в поисках свободного
	got_direction = false;

	for (i = 0; i < 4; i++)
	{
		switch (direction)
		{
		case EDirection_Type::Left:
			if (dest_brick_x > 0 && (EBrick_Type)Current_Level[dest_brick_y][dest_brick_x - 1] == EBrick_Type::None)
				got_direction = true;
			break;

		case EDirection_Type::Up:
			if (dest_brick_y > 0 && (EBrick_Type)Current_Level[dest_brick_y - 1][dest_brick_x] == EBrick_Type::None)
				got_direction = true;
			break;

		case EDirection_Type::Right:
			if (dest_brick_x < AsConfig::Level_Width - 1 && (EBrick_Type)Current_Level[dest_brick_y][dest_brick_x + 1] == EBrick_Type::None)
				got_direction = true;
			break;

		case EDirection_Type::Down:
			if (dest_brick_y < AsConfig::Level_Height - 1 && (EBrick_Type)Current_Level[dest_brick_y + 1][dest_brick_x] == EBrick_Type::None)
				got_direction = true;
			break;

		default:
			AsConfig::Throw();
		}

		if (got_direction)
			break;

		direction = (EDirection_Type)( (int)direction - 1);
		ball->Set_Direction(ball->Get_Direction() + M_PI_2);

		if ( (int)direction < 0)
			direction = EDirection_Type::Down;
	}

	destination_teleport->Release_Direction = direction;

	Active_Bricks.push_back(source_teleport);
	Active_Bricks.push_back(destination_teleport);
}
//------------------------------------------------------------------------------------------------------------
AActive_Brick_Teleport *AsLevel::Select_Destination_Teleport(int source_x, int source_y)
{
	int dest_index;

	AActive_Brick_Teleport *destination_teleport;

	if (Teleport_Bricks_Pos.size() < 2)
	{
		AsConfig::Throw();
		return 0;
	}

	dest_index = AsTools::Rand(Teleport_Bricks_Pos.size() );

	if (Teleport_Bricks_Pos[dest_index].X == source_x && Teleport_Bricks_Pos[dest_index].Y == source_y)
	{// Если случайно выбрали текущий телепорт - переходим к следующему

		++dest_index;

		if (dest_index >= (int)Teleport_Bricks_Pos.size() )
			dest_index = 0;  // Переходим на начало массива, если вышли за его пределы
	}

	destination_teleport = new AActive_Brick_Teleport(Teleport_Bricks_Pos[dest_index].X, Teleport_Bricks_Pos[dest_index].Y, 0, 0);

	return destination_teleport;
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Check_Vertical_Hit(double next_x_pos, double next_y_pos, int level_x, int level_y, ABall_Object *ball, double &reflection_pos)
{
	double direction = ball->Get_Direction();

	if (ball->Is_Moving_Up() )
	{// Проверяем попадание в нижнюю грань

		if (Hit_Circle_On_Line(next_y_pos - Current_Brick_Low_Y, next_x_pos, Current_Brick_Left_X, Current_Brick_Right_X, AsConfig::Ball_Radius, reflection_pos) )
		{
			// Проверяем возможность отскока вниз
			if (level_y < AsConfig::Level_Height - 1 && Current_Level[level_y + 1][level_x] == 0)
				return true;
			else
				return false;
		}
	}
	else
	{// Проверяем попадание в верхнюю грань

		if (Hit_Circle_On_Line(next_y_pos - Current_Brick_Top_Y, next_x_pos, Current_Brick_Left_X, Current_Brick_Right_X, AsConfig::Ball_Radius, reflection_pos) )
		{
			// Проверяем возможность отскока вверх
			if (level_y > 0 && Current_Level[level_y - 1][level_x] == 0)
				return true;
			else
				return false;
		}
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------
bool AsLevel::Check_Horizontal_Hit(double next_x_pos, double next_y_pos, int level_x, int level_y, ABall_Object *ball, double &reflection_pos)
{
	double direction = ball->Get_Direction();

	if (! ball->Is_Moving_Left() )
	{// Проверяем попадание в левую грань

		if (Hit_Circle_On_Line(Current_Brick_Left_X - next_x_pos, next_y_pos, Current_Brick_Top_Y, Current_Brick_Low_Y, AsConfig::Ball_Radius, reflection_pos) )
		{
			// Проверяем возможность отскока влево
			if (level_x > 0 && Current_Level[level_y][level_x - 1] == 0)
				return true;
			else
				return false;
		}
	}
	else
	{// Проверяем попадание в правую грань

		if (Hit_Circle_On_Line(Current_Brick_Right_X - next_x_pos, next_y_pos, Current_Brick_Top_Y, Current_Brick_Low_Y, AsConfig::Ball_Radius, reflection_pos) )
		{
			// Проверяем возможность отскока вправо
			if (level_x < AsConfig::Level_Width - 1 && Current_Level[level_y][level_x + 1] == 0)
				return true;
			else
				return false;
		}
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Draw_Brick(HDC hdc, RECT &brick_rect, int level_x, int level_y)
{// Вывод "кирпича"

	const AColor *color = 0;
	EBrick_Type brick_type = (EBrick_Type)Current_Level[level_y][level_x];

	switch (brick_type)
	{
	case EBrick_Type::None:
		if (Advertisement != 0 && Advertisement->Has_Brick_At(level_x, level_y) )
			break;
		// else - No break!

	case EBrick_Type::Red:
	case EBrick_Type::Blue:
		AActive_Brick_Red_Blue::Draw_In_Level(hdc, brick_rect, brick_type);
		break;

	case EBrick_Type::Unbreakable:
		AActive_Brick_Unbreakable::Draw_In_Level(hdc, brick_rect);
		break;

	case EBrick_Type::Multihit_1:
	case EBrick_Type::Multihit_2:
	case EBrick_Type::Multihit_3:
	case EBrick_Type::Multihit_4:
		AActive_Brick_Multihit::Draw_In_Level(hdc, brick_rect, brick_type);
		break;

	case EBrick_Type::Parachute:
		Draw_Parachute_In_Level(hdc, brick_rect);
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
		AsConfig::Throw();
	}
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Draw_Parachute_In_Level(HDC hdc, RECT &brick_rect)
{
	Draw_Parachute_Part(hdc, brick_rect, 0, 4);
	Draw_Parachute_Part(hdc, brick_rect, 4, 6);
	Draw_Parachute_Part(hdc, brick_rect, 10, 4);
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Draw_Parachute_Part(HDC hdc, RECT &brick_rect, int offset, int width)
{
	const int scale = AsConfig::Global_Scale;
	RECT rect;

	// 1. Верхний сегмент
	rect.left = brick_rect.left + offset * scale + 1;
	rect.top = brick_rect.top + 1;
	rect.right = rect.left + width * scale + 1;
	rect.bottom = rect.top + 3 * scale + 1;

	Parachute_Color.Select(hdc);
	AsTools::Round_Rect(hdc, rect);

	// 2. Нижний сегмент
	rect.top += 3 * scale;
	rect.bottom += 3 * scale;

	AsTools::Round_Rect(hdc, rect);
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Delete_Objects(std::vector<AGraphics_Object *> &falling_letters)
{
	for (auto it = falling_letters.begin(); it != falling_letters.end(); it++)
		delete *it;

	falling_letters.erase(falling_letters.begin(), falling_letters.end() );
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Act_Objects(std::vector<AGraphics_Object *> &falling_letters)
{
	auto it = falling_letters.begin();

	while (it != falling_letters.end() )
	{
		(*it)->Act();

		if ( (*it)->Is_Finished() )
		{
			delete *it;
			it = falling_letters.erase(it);
		}
		else
			it++;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsLevel::Cancel_All_Activity()
{
	Delete_Objects(Active_Bricks);
	Delete_Objects(Falling_Letters);
}
//------------------------------------------------------------------------------------------------------------
