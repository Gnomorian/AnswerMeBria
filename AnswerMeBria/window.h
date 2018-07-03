#pragma once

#include "common.h"

namespace Window {
	static LPCSTR windowClass;
	static LPCSTR title;
	// the window properties
	static WNDCLASSEX wcex;
	// handle to the window
	static HWND hwnd;
	// hook to get low level keyboard events
	static HHOOK hookproc = NULL;
	// tells the hook when to look for keyboard events
	static bool sessionLocked = false;

	extern int InitWindow(LPCSTR windowClass, LPCSTR title, HINSTANCE instance);

	// hwnd - handle of the window
	// uMsg - message your window has recieved.
	// wparam - extra params for the message
	// lParam - extra params for the message
	// https://msdn.microsoft.com/en-us/library/windows/desktop/ms633573(v=vs.85).aspx
	extern LRESULT CALLBACK WndProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

	// a callback used with SetWindowsHookEx function. the system calls this function 
	// everytime a new kjeyboard input event is about to be posted into the thread input queue.
	// nCode - A code the hook procedure uses to determine how to process the message.
	// wParam - The identifier of the keyboard message.This parameter can be one of the following messages : WM_KEYDOWN, WM_KEYUP, WM_SYSKEYDOWN, or WM_SYSKEYUP.
	// lParam - A pointer to a KBDLLHOOKSTRUCT structure.
	// https://msdn.microsoft.com/en-us/library/windows/desktop/ms644985%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
	extern LRESULT CALLBACK LowLevelKeyboardProc(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam);

	extern void Show(int CmdShow);

	extern bool RegisterKeyboardHook();

	extern void Worker(LPARAM lParam);

	extern void Cleanup();
}