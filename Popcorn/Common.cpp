#include "Common.h"

// AMover
//------------------------------------------------------------------------------------------------------------
AMover::~AMover()
{
}
//------------------------------------------------------------------------------------------------------------
AMover::AMover()
	: Speed(0.0)
{
}
//------------------------------------------------------------------------------------------------------------




// AGraphics_Object
//------------------------------------------------------------------------------------------------------------
AGraphics_Object::~AGraphics_Object()
{}
//------------------------------------------------------------------------------------------------------------




// AGame_Objects_Set
//------------------------------------------------------------------------------------------------------------
void AGame_Objects_Set::Begin_Movement()
{
	int index = 0;
	AGame_Object *game_obj;

	while(Get_Next_Obj(index, &game_obj) )
		game_obj->Begin_Movement();
}
//------------------------------------------------------------------------------------------------------------
void AGame_Objects_Set::Finish_Movement()
{
	int index = 0;
	AGame_Object *game_obj;

	while(Get_Next_Obj(index, &game_obj) )
		game_obj->Finish_Movement();
}
//------------------------------------------------------------------------------------------------------------
void AGame_Objects_Set::Advance(double max_speed)
{
	int index = 0;
	AGame_Object *game_obj;

	while(Get_Next_Obj(index, &game_obj) )
		game_obj->Advance(max_speed);
}
//------------------------------------------------------------------------------------------------------------
double AGame_Objects_Set::Get_Speed()
{
	int index = 0;
	double max_speed = 0.0;
	double curr_speed;
	AGame_Object *game_obj;

	while(Get_Next_Obj(index, &game_obj) )
	{
		curr_speed = fabs(game_obj->Get_Speed() );

		if (curr_speed > max_speed)
			max_speed = curr_speed;
	}

	return max_speed;
}
//------------------------------------------------------------------------------------------------------------
void AGame_Objects_Set::Act()
{
	int index = 0;
	AGame_Object *game_obj;

	while(Get_Next_Obj(index, &game_obj) )
		game_obj->Act();
}
//------------------------------------------------------------------------------------------------------------
void AGame_Objects_Set::Clear(HDC hdc, RECT &paint_area)
{
	int index = 0;
	AGame_Object *game_obj;

	while(Get_Next_Obj(index, &game_obj) )
		game_obj->Clear(hdc, paint_area);
}
//------------------------------------------------------------------------------------------------------------
void AGame_Objects_Set::Draw(HDC hdc, RECT &paint_area)
{
	int index = 0;
	AGame_Object *game_obj;

	while(Get_Next_Obj(index, &game_obj) )
		game_obj->Draw(hdc, paint_area);
}
//------------------------------------------------------------------------------------------------------------
bool AGame_Objects_Set::Is_Finished()
{
	return false;  // Этот метод пока не используется ни в одном из объектов
}
//------------------------------------------------------------------------------------------------------------




// AString
//-----------------------------------------------------------------------------------------------------------
AString::AString()
{
}
//-----------------------------------------------------------------------------------------------------------
AString::AString(const wchar_t *str)
	: Content(str)
{
}
//-----------------------------------------------------------------------------------------------------------
void AString::Append(int value)
{
	wchar_t buf[32]{};

	swprintf(buf, 32, L"%.6i", value);
	Content += buf;
}
//-----------------------------------------------------------------------------------------------------------
const wchar_t *AString::Get_Content()
{
	return Content.c_str();
}
//-----------------------------------------------------------------------------------------------------------
int AString::Get_Length()
{
	return (int)Content.length();
}
//-----------------------------------------------------------------------------------------------------------




// AMessage
//------------------------------------------------------------------------------------------------------------
AMessage::AMessage(EMessage_Type message_type)
	: Type(message_type)
{
}
//------------------------------------------------------------------------------------------------------------




// AsMessage_Manager
std::queue<AMessage *> AsMessage_Manager::Message_Queue;
//------------------------------------------------------------------------------------------------------------
void AsMessage_Manager::Add_Message(AMessage *message)
{
	Message_Queue.push(message);
}
//------------------------------------------------------------------------------------------------------------
bool AsMessage_Manager::Get_Message(AMessage **message)
{
	if (Message_Queue.size() == 0)
		return false;

	*message = Message_Queue.front();

	Message_Queue.pop();

	return true;
}
//------------------------------------------------------------------------------------------------------------
