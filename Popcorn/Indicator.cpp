#include "Indicator.h"

// AIndicator
//-----------------------------------------------------------------------------------------------------------
AIndicator::AIndicator(EMessage_Type message_type, int x_pos, int y_pos)
	: Message_Type(message_type), Need_To_Notify(false), X_Pos(x_pos), Y_Pos(y_pos), Indicator_Timer_Tick(0)
{
	Indicator_Rect.left = X_Pos * AsConfig::Global_Scale;
	Indicator_Rect.top = Y_Pos * AsConfig::Global_Scale;
	Indicator_Rect.right = Indicator_Rect.left + Indicator_Width * AsConfig::Global_Scale;
	Indicator_Rect.bottom = Indicator_Rect.top + Indicator_Height * AsConfig::Global_Scale;
}
//-----------------------------------------------------------------------------------------------------------
void AIndicator::Act()
{
	if (! Is_Finished() )
		AsTools::Invalidate_Rect(Indicator_Rect);
}
//-----------------------------------------------------------------------------------------------------------
void AIndicator::Clear(HDC hdc, RECT &paint_area)
{
	//  Заглушка. Этот метод не используется
}
//-----------------------------------------------------------------------------------------------------------
void AIndicator::Draw(HDC hdc, RECT &paint_area)
{
	int inner_x_offset = (Indicator_Width - Inner_Width) / 2;
	int inner_y_offset = (Indicator_Height - Inner_Height) / 2;
	int curr_height;
	const int scale = AsConfig::Global_Scale;
	double ratio;
	RECT rect;

	AsTools::Rect(hdc, X_Pos, Y_Pos, Indicator_Width, Indicator_Height, AsConfig::Floor_Indicator_Color);

	if (Indicator_Timer_Tick == 0 || Is_Finished() )
		return;

	ratio = (double)(Indicator_Timer_Tick - AsConfig::Current_Timer_Tick) / (double)Indicator_Timeout;

	curr_height = (int)( (double)(Inner_Height * scale) * ratio);

	if (curr_height == 0)
		return;

	rect.left = (X_Pos + inner_x_offset) * scale;
	rect.top = (Y_Pos + inner_y_offset) * scale + (Inner_Height * scale - curr_height);
	rect.right = rect.left + Inner_Width * scale;
	rect.bottom = (Y_Pos + inner_y_offset + Inner_Height) * scale;

	AsTools::Rect(hdc, rect, AsConfig::White_Color);
}
//-----------------------------------------------------------------------------------------------------------
bool AIndicator::Is_Finished()
{
	if (AsConfig::Current_Timer_Tick > Indicator_Timer_Tick)
	{
		Cancel();
		return true;
	}
	else
		return false;
}
//-----------------------------------------------------------------------------------------------------------
void AIndicator::Restart()
{
	Need_To_Notify = true;

	Indicator_Timer_Tick = AsConfig::Current_Timer_Tick + Indicator_Timeout;
}
//-----------------------------------------------------------------------------------------------------------
void AIndicator::Cancel()
{
	AMessage *message;

	if (Need_To_Notify)
	{
		message = new AMessage(Message_Type);

		AsMessage_Manager::Add_Message(message);

		Need_To_Notify = false;
	}

	Reset();
}
//-----------------------------------------------------------------------------------------------------------
void AIndicator::Reset()
{
	Indicator_Timer_Tick = 0;
	AsTools::Invalidate_Rect(Indicator_Rect);
}
//-----------------------------------------------------------------------------------------------------------
