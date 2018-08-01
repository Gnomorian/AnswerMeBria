#pragma once

#include <Windows.h>
#include <shellapi.h>
#include "resource.h"
#include <string>

namespace Tray
{
	static UINT WM_TASKBAR;
	static HWND Hwnd;
	static HWND hEdit;
	static HMENU Hmenu;
	static NOTIFYICONDATA notifyIconData;
	static TCHAR szTIP[64] = TEXT("AnswerMeBria!");
	static std::string className;

	static WNDCLASSEX wincl; /*data structure for the windowclass*/

	extern LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	extern void minimize();
	extern void restore();
	extern void InitNotifyIconData();
	extern int Initialize(HINSTANCE thisInstance, int cmdShow);
};

