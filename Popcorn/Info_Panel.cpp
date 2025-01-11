#include "Info_Panel.h"

// AsInfo_Panel
int AsInfo_Panel::Score = 0;
RECT AsInfo_Panel::Score_Rect{};
//-----------------------------------------------------------------------------------------------------------
AsInfo_Panel::~AsInfo_Panel()
{
	delete Shadow_Color;
	delete Highlight_Color;

	if (Logo_Pop_Font != 0)
		DeleteObject(Logo_Pop_Font);

	if (Logo_Corn_Font != 0)
		DeleteObject(Logo_Corn_Font);

	if (Player_Name_Font != 0)
		DeleteObject(Player_Name_Font);
}
//-----------------------------------------------------------------------------------------------------------
AsInfo_Panel::AsInfo_Panel()
: Extra_Lives_Count(AsConfig::Initial_Life_Count), Shadow_Color(0), Highlight_Color(0), Logo_Pop_Font(0), Logo_Corn_Font(0), Player_Name_Font(0),
  Letter_F(EBrick_Type::Blue, ELetter_Type::F, 216, 155),
  Letter_M(EBrick_Type::Blue, ELetter_Type::M, 296, 155),
  Letter_Plus(EBrick_Type::Blue, ELetter_Type::Plus, 256, 155), Player_Name(L"COMPUTER"),
  Floor_Indicator(EMessage_Type::Floor_Is_Ends, Indicators_Panel_X_Pos + 9, Indicators_Panel_Y_Pos + 56),
  Monster_Indicator(EMessage_Type::Unfreeze_Monster, Indicators_Panel_X_Pos + 89, Indicators_Panel_Y_Pos + 56)
{
	const int scale = AsConfig::Global_Scale;

	Logo_Rect.left = Logo_X_Pos * scale;
	Logo_Rect.top = Logo_Y_Pos * scale;
	Logo_Rect.right = Logo_Rect.left + 104 * scale;
	Logo_Rect.bottom = Logo_Rect.top + 100 * scale;

	Indicators_Panel_Rect.left = (Indicators_Panel_X_Pos + 2) * scale;
	Indicators_Panel_Rect.top = (Indicators_Panel_Y_Pos + 2) * scale;
	Indicators_Panel_Rect.right = Indicators_Panel_Rect.left + (Indicators_Panel_Width - 4) * scale;
	Indicators_Panel_Rect.bottom = Indicators_Panel_Rect.top + (Indicators_Panel_Heigth - 4) * scale;

	Name_Rect.left = (Indicators_Panel_X_Pos + 5) * scale;
	Name_Rect.top = (Indicators_Panel_Y_Pos + 5) * scale;
	Name_Rect.right = Name_Rect.left + (Indicators_Panel_Width - 2 * 5) * scale;
	Name_Rect.bottom = Name_Rect.top + 16 * scale;

	Score_Rect.left = Name_Rect.left;
	Score_Rect.top = Name_Rect.top + 22 * scale;
	Score_Rect.right = Name_Rect.right;
	Score_Rect.bottom = Name_Rect.bottom + 22 * scale;

	Letter_F.Show_Full_Size();
	Letter_M.Show_Full_Size();
	Letter_Plus.Show_Full_Size();
}
//-----------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Begin_Movement()
{
	//  Заглушка. Этот метод не используется
}
//-----------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Finish_Movement()
{
	//  Заглушка. Этот метод не используется
}
//-----------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Advance(double max_speed)
{
	//  Заглушка. Этот метод не используется
}
//-----------------------------------------------------------------------------------------------------------
double AsInfo_Panel::Get_Speed()
{
	return 0.0;  //  Заглушка. Этот метод не используется
}
//-----------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Act()
{
	Floor_Indicator.Act();
	Monster_Indicator.Act();
}
//-----------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Clear(HDC hdc, RECT &paint_area)
{
	//  Заглушка. Этот метод не используется
}
//-----------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Draw(HDC hdc, RECT &paint_area)
{
	Draw_Logo(hdc, paint_area);

	// Таблица счета
	Draw_Indicators_Panel(hdc, paint_area);

	// 3. Индикаторы
	// 3.1 Пол
	Letter_F.Draw(hdc, paint_area);
	Floor_Indicator.Draw(hdc, paint_area);

	// 3.2 Жизни
	Letter_Plus.Draw(hdc, paint_area);
	Show_Extra_Lives(hdc);

	// 3.3 Монстр
	Letter_M.Draw(hdc, paint_area);
	Monster_Indicator.Draw(hdc, paint_area);
}
//-----------------------------------------------------------------------------------------------------------
bool AsInfo_Panel::Is_Finished()
{
	return false;  // Заглушка. Этот метод не используется
}
//-----------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Init()
{
	LOGFONT log_font{}; 

	log_font.lfHeight = -128;
	log_font.lfWeight = 900;
	log_font.lfOutPrecision = 3;
	log_font.lfClipPrecision = 2;
	log_font.lfQuality = 1;
	log_font.lfPitchAndFamily = 34;

	wcscpy_s(log_font.lfFaceName, L"Arial Black");

	Logo_Pop_Font = CreateFontIndirect(&log_font); 

	log_font.lfHeight = -96;
	Logo_Corn_Font = CreateFontIndirect(&log_font);

	log_font.lfHeight = -48;
	log_font.lfWeight = 700;
	log_font.lfOutPrecision = 3;
	log_font.lfClipPrecision = 2;
	log_font.lfQuality = 1;
	log_font.lfPitchAndFamily = 49;

	wcscpy_s(log_font.lfFaceName, L"Consolas");

	log_font.lfHeight = -44;
	Player_Name_Font = CreateFontIndirect(&log_font); 

	Shadow_Color = new AColor(AsConfig::BG_Color, AsConfig::Global_Scale);
	Highlight_Color = new AColor(AsConfig::White_Color, AsConfig::Global_Scale);
}
//-----------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Increase_Life_Count()
{
	if (Extra_Lives_Count < AsConfig::Max_Life_Count)
	{
		++Extra_Lives_Count;
		AsTools::Invalidate_Rect(Indicators_Panel_Rect);
	}
}
//-----------------------------------------------------------------------------------------------------------
bool AsInfo_Panel::Decrease_Life_Count()
{
	if (Extra_Lives_Count == 0)
		return false;

	--Extra_Lives_Count;
	AsTools::Invalidate_Rect(Indicators_Panel_Rect);
	
	return true;
}
//-----------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Update_Score(EScore_Event_Type event_type)
{
	switch (event_type)
	{
	case EScore_Event_Type::Hit_Brick:
		Score += 20;
		break;

	case EScore_Event_Type::Hit_Monster:
		Score += 37;
		break;

	case EScore_Event_Type::Catch_Letter:
		Score += 23;
		break;

	default:
		AsTools::Throw();
	}

	AsTools::Invalidate_Rect(Score_Rect);
}
//-----------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Choose_Font()
{
	CHOOSEFONT cf{}; 
	LOGFONT lf{};

	cf.lStructSize = sizeof(CHOOSEFONT); 
	cf.lpLogFont = &lf; 
	cf.Flags = CF_SCREENFONTS; 
	cf.nFontType = SCREEN_FONTTYPE; 

	ChooseFont(&cf); 
}
//-----------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Show_Extra_Lives(HDC hdc)
{
	int i, j;

	int lives_to_draw = Extra_Lives_Count;

	AsTools::Round_Rect(hdc, Indicators_Panel_X_Pos + 28, Indicators_Panel_Y_Pos + 56, 55, 30, AsConfig::Floor_Indicator_Color);

	for (j = 0; j < 3; j++)
		for (i = 0; i < 4; i++)
		{
			if (lives_to_draw <= 0)
				break;

			Draw_Extra_Life(hdc, 27 + 6 + j * 16, 58 + i * 7);
			--lives_to_draw;
		}
}
//-----------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Draw_Logo(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if (! IntersectRect(&intersection_rect, &paint_area, &Logo_Rect) )
		return;

	const int scale = AsConfig::Global_Scale;
	int pop_x_pos = (Logo_X_Pos - 8) * scale;
	int pop_y_pos = Logo_Y_Pos * scale;
	int corn_x_pos = (Logo_X_Pos - 13) * scale;
	int corn_y_pos = pop_y_pos + 44 * scale;
	const wchar_t *pop_str = L"POP";
	const wchar_t *corn_str = L"CORN";

	// 1. Логотип
	AsTools::Rect(hdc, 212, 7, 102, 99, AsConfig::Blue_Color);

	SetBkMode(hdc, TRANSPARENT);

	// 1.1 Тень
	SetTextColor(hdc, AsConfig::BG_Color.Get_RGB() );

	SelectObject(hdc, Logo_Pop_Font);
	TextOut(hdc, pop_x_pos + Shadow_X_Offset, pop_y_pos + Shadow_Y_Offset, pop_str, (int)wcslen(pop_str));
	SelectObject(hdc, Logo_Corn_Font);
	TextOut(hdc, corn_x_pos + Shadow_X_Offset, corn_y_pos + Shadow_Y_Offset, corn_str, (int)wcslen(corn_str));

	// 1.2 Сами символы
	SetTextColor(hdc, AsConfig::Red_Color.Get_RGB() );

	SelectObject(hdc, Logo_Pop_Font);
	TextOut(hdc, pop_x_pos, pop_y_pos, pop_str, (int)wcslen(pop_str));
	SelectObject(hdc, Logo_Corn_Font);
	TextOut(hdc, corn_x_pos, corn_y_pos, corn_str, (int)wcslen(corn_str));
}
//-----------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Draw_Extra_Life(HDC hdc, int x_pos, int y_pos)
{
	const int scale = AsConfig::Global_Scale;
	RECT rect;

	rect.left = (Indicators_Panel_X_Pos + x_pos) * scale + 1;
	rect.top = (Indicators_Panel_Y_Pos + y_pos) * scale;
	rect.right = rect.left + 4 * scale;
	rect.bottom = rect.top + 5 * scale;

	AsTools::Ellipse(hdc, rect, AsConfig::Red_Color);

	rect.left += 8 * scale;
	rect.right += 8 * scale;

	AsTools::Ellipse(hdc, rect, AsConfig::Red_Color);

	rect.left = (Indicators_Panel_X_Pos + x_pos + 2) * scale + 1;
	rect.top = (Indicators_Panel_Y_Pos + y_pos + 1) * scale;
	rect.right = rect.left + 8 * scale;
	rect.bottom = rect.top + 3 * scale;

	AsConfig::Blue_Color.Select(hdc);
	AsTools::Round_Rect(hdc, rect);
}
//------------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Draw_Indicators_Panel(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if (! IntersectRect(&intersection_rect, &paint_area, &Indicators_Panel_Rect) )
		return;

	const int scale = AsConfig::Global_Scale;
	int distance_to_shadow = 4 * scale;

	// Панель
	AsTools::Rect(hdc, Indicators_Panel_X_Pos, Indicators_Panel_Y_Pos, Indicators_Panel_Width, 2, AsConfig::Red_Color);
	AsTools::Rect(hdc, Indicators_Panel_X_Pos, Indicators_Panel_Y_Pos, 2, Indicators_Panel_Heigth, AsConfig::Red_Color);
	AsTools::Rect(hdc, Indicators_Panel_X_Pos, Indicators_Panel_Y_Pos + Indicators_Panel_Heigth - 2, Indicators_Panel_Width, 2, AsConfig::Red_Color);
	AsTools::Rect(hdc, Indicators_Panel_X_Pos + Indicators_Panel_Width - 2, Indicators_Panel_Y_Pos, 2, Indicators_Panel_Heigth, AsConfig::Red_Color);

	// Синяя панель
	Draw_Rect_With_Shadow(hdc, Indicators_Panel_Rect, AsConfig::Blue_Color);

	// Панель с именем игрока
	Draw_Player_Name_Panel(hdc,paint_area);

	// Панель счета
	Draw_Score_Panel(hdc, paint_area);
}
//-----------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Draw_Player_Name_Panel(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if (! IntersectRect(&intersection_rect, &paint_area, &Name_Rect) )
		return;

	const int scale = AsConfig::Global_Scale;
	int distance_to_shadow = 4 * scale;
	RECT name_shadow_rect{};

	Get_Shadow_Rect(Name_Rect, name_shadow_rect, distance_to_shadow);

	Draw_Rect_With_Shadow(hdc, Name_Rect, AsConfig::Red_Color);

	Draw_String(hdc, name_shadow_rect, Player_Name_Font, Player_Name, AsConfig::BG_Color);
	Draw_String(hdc, Name_Rect, Player_Name_Font, Player_Name, AsConfig::Blue_Color);
}
//-----------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Draw_Score_Panel(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if (! IntersectRect(&intersection_rect, &paint_area, &Score_Rect) )
		return;

	const int scale = AsConfig::Global_Scale;
	int distance_to_shadow = 4 * scale;
	RECT score_shadow_rect{};
	AString score_str( L"SCORE:");

	Get_Shadow_Rect(Score_Rect, score_shadow_rect, distance_to_shadow);

	Draw_Rect_With_Shadow(hdc, Score_Rect, AsConfig::Red_Color);

	score_str.Append(Score);

	Draw_String(hdc, score_shadow_rect, Player_Name_Font, score_str, AsConfig::BG_Color);
	Draw_String(hdc, Score_Rect, Player_Name_Font, score_str, AsConfig::White_Color);
}
//-----------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Draw_String(HDC hdc, RECT &str_rect, HFONT &font, AString &str, const AColor &color)
{
	int str_x_pos, str_y_pos;
	SIZE str_size{};

	SelectObject(hdc, font);
	SetTextColor(hdc, color.Get_RGB() );

	GetTextExtentPoint32(hdc, str.Get_Content(), str.Get_Length(), &str_size);

	str_x_pos = str_rect.left + (str_rect.right - str_rect.left) / 2 - str_size.cx / 2;
	str_y_pos = str_rect.top + (str_rect.bottom - str_rect.top) / 2 - str_size.cy / 2;

	TextOut(hdc, str_x_pos, str_y_pos, str.Get_Content(), str.Get_Length() );
}
//-----------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Draw_Rect_With_Shadow(HDC hdc, RECT &panel_rect, const AColor &panel_color)
{
	AsTools::Rect(hdc, panel_rect, panel_color);

	Shadow_Color->Select_Pen(hdc);
	MoveToEx(hdc, panel_rect.right, panel_rect.top, 0);
	LineTo(hdc, panel_rect.right, panel_rect.bottom);
	LineTo(hdc, panel_rect.left, panel_rect.bottom);

	Highlight_Color->Select_Pen(hdc);
	LineTo(hdc, panel_rect.left, panel_rect.top);
	LineTo(hdc, panel_rect.right, panel_rect.top);

}
//-----------------------------------------------------------------------------------------------------------
void AsInfo_Panel::Get_Shadow_Rect(RECT &src_rect, RECT &dst_rect, int distance_to_shadow)
{
	dst_rect.left = src_rect.left + Shadow_X_Offset - distance_to_shadow;
	dst_rect.top = src_rect.top + Shadow_Y_Offset - distance_to_shadow;
	dst_rect.right = src_rect.right + Shadow_X_Offset - distance_to_shadow;
	dst_rect.bottom = src_rect.bottom + Shadow_Y_Offset - distance_to_shadow;
}
//-----------------------------------------------------------------------------------------------------------
