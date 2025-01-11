#pragma once

#include "Config.h"

//-----------------------------------------------------------------------------------------------------------
class AIndicator: public AGraphics_Object
{
public:
	AIndicator(EMessage_Type message_type, int x_pos, int y_pos);

	virtual void Act();
	virtual void Clear(HDC hdc, RECT &paint_area);
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual bool Is_Finished();

	void Restart();
	void Cancel();
	void Reset();

private:
	EMessage_Type Message_Type;

	bool Need_To_Notify;
	int X_Pos, Y_Pos;
	int Indicator_Timer_Tick;

	RECT Indicator_Rect;

	static const int Indicator_Width = 13;
	static const int Indicator_Height = 30;
	static const int Inner_Width = Indicator_Width - 2;
	static const int Inner_Height = Indicator_Height - 2;
	static const int Indicator_Timeout = 50 * AsConfig::FPS;
};
//-----------------------------------------------------------------------------------------------------------
