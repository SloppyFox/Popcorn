#pragma once

#include "resource.h"
#include "Engine.h"

//------------------------------------------------------------------------------------------------------------
class AsFrame_DC
{
public:
	~AsFrame_DC();
	AsFrame_DC();

	HDC Get_DC(HWND hwnd, HDC hdc);

	int Width, Height;

private:
	HDC DC;
	HBITMAP Bitmap;
};
//------------------------------------------------------------------------------------------------------------
class AsMain_Window
{
public:
	AsMain_Window();

	int APIENTRY Main(HINSTANCE instance, int command_show);

private:
	ATOM Register_Class();
	BOOL Init_Instance(int command_show);
	void On_Paint(HWND hwnd);

	static const int Max_Load_String = 100;

	AsFrame_DC Frame_DC;
	AsEngine Engine;
	HINSTANCE Instance;  // current instance
	WCHAR szTitle[Max_Load_String];  // The title bar text
	WCHAR szWindowClass[Max_Load_String];  // the main window class name

	static LRESULT CALLBACK Window_Proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
	static INT_PTR CALLBACK About(HWND hdlg, UINT message, WPARAM wparam, LPARAM lparam);

	static AsMain_Window *Self;
};
//------------------------------------------------------------------------------------------------------------
