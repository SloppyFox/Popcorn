#include "Monster_Set.h"

// AsMonster_Set
//------------------------------------------------------------------------------------------------------------
AsMonster_Set::~AsMonster_Set()
{
	for (auto *curr_monster : Monsters)
		delete curr_monster;

	Monsters.erase(Monsters.begin(), Monsters.end() );
}
//------------------------------------------------------------------------------------------------------------
AsMonster_Set::AsMonster_Set()
: Monster_Set_State(EMonster_Set_State::Idle), Is_Frozen(false), Current_Gate(0),
  Max_Alive_Monsters_Count(0), Border(0)
{
}
//------------------------------------------------------------------------------------------------------------
bool AsMonster_Set::Check_Hit(double next_x_pos, double next_y_pos, ABall_Object *ball)
{
	for (auto *curr_monster : Monsters)
		if (curr_monster->Check_Hit(next_x_pos, next_y_pos, ball) )
			return true;

	return false;
}
//------------------------------------------------------------------------------------------------------------
bool AsMonster_Set::Check_Hit(double next_x_pos, double next_y_pos)
{
	for (auto *curr_monster : Monsters)
		if (curr_monster->Check_Hit(next_x_pos, next_y_pos) )
			return true;

	return false;
}
//------------------------------------------------------------------------------------------------------------
bool AsMonster_Set::Check_Hit(RECT &rect)
{
	for (auto *curr_monster : Monsters)
		if (curr_monster->Check_Hit(rect) )
			return true;

	return false;
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Act()
{
	int curr_alive_monsters_count;

	switch (Monster_Set_State)
	{
	case EMonster_Set_State::Idle:
		break;


	case EMonster_Set_State::Select_Next_Gate:
		if (Is_Frozen)
			return;
		
		curr_alive_monsters_count = 0;
		for (auto *curr_monster : Monsters)
			if ( ! curr_monster->Is_Finished() )
				++curr_alive_monsters_count;

		if(curr_alive_monsters_count < Max_Alive_Monsters_Count)
		{
			Current_Gate = Border->Long_Open_Gate();
			Monster_Set_State = EMonster_Set_State::Waitinig_For_Open_Gate;
		}
		break;


	case EMonster_Set_State::Waitinig_For_Open_Gate:
		if (Border->Is_Gate_Opened(Current_Gate) )
		{
			Let_Out(Current_Gate);
			Monster_Set_State = EMonster_Set_State::Waitinig_For_Close_Gate;
		}
		break;


	case EMonster_Set_State::Waitinig_For_Close_Gate:
		if (Border->Is_Gate_Close(Current_Gate) )
			Monster_Set_State = EMonster_Set_State::Select_Next_Gate;
		break;


	default:
		AsTools::Throw();
	}

	if (Monster_Set_State != EMonster_Set_State::Idle)
	{
		auto it = Monsters.begin();

		while (it != Monsters.end() )
			if ( (*it)->Is_Finished() )
			{
				delete* it;
				it = Monsters.erase(it);
			}
			else
				it++;
	}

	AGame_Objects_Set::Act();
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Init(AsBorder *border)
{
	if (border == 0)
		AsTools::Throw();

	Border = border;
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Activate(int max_alive_monsters_count)
{
	Max_Alive_Monsters_Count = max_alive_monsters_count;
	Monster_Set_State = EMonster_Set_State::Select_Next_Gate;
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Let_Out(int gate_index)
{
	bool is_gate_left;
	int gate_x_pos, gate_y_pos;
	AMonster *curr_monster;

	if (Is_Frozen)
		return;

	if (Monsters.size() >= AsConfig::Max_Monsters_Count)
		return;

	if (AsTools::Rand(2) == 0)
		curr_monster = new AMonster_Eye();
	else
		curr_monster = new AMonster_Comet();

	Monsters.push_back(curr_monster);

	if (gate_index % 2 == 0)
		is_gate_left = true;
	else
		is_gate_left = false;

	Border->Get_Gate_Pos(gate_index, gate_x_pos, gate_y_pos);

	curr_monster->Let_Out(gate_x_pos, gate_y_pos, is_gate_left);
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Destroy_All()
{
	for (auto *curr_monster : Monsters)
		curr_monster->Destroy();

	Monster_Set_State = EMonster_Set_State::Idle;
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Set_Freeze_State(bool is_freeze)
{
	Is_Frozen = is_freeze;

	for (auto *curr_monster : Monsters)
		curr_monster->Set_Freeze_State(is_freeze);
}
//------------------------------------------------------------------------------------------------------------
bool AsMonster_Set::Get_Next_Obj(int &index, AGame_Object **game_obj)
{
	AMonster *curr_monster = 0;

	if (index < 0 || index >= (int)Monsters.size() )
		return false;

	*game_obj = Monsters[index++];

	return true;
}
//------------------------------------------------------------------------------------------------------------