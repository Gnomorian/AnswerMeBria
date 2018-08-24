#pragma once

#include "common.h"
#include "Registry.h"

namespace Window {
	static LPCSTR windowClass;
	static LPCSTR title;
	// the window properties
	static WNDCLASSEX wcex;
	// handle? to the window messages
	static UINT WM_TASKBAR;
	// handle to the window
	static HWND hwnd;
	// the tray menu
	static HMENU Hmenu;
	// notifyicondata, data for the tray icon
	static NOTIFYICONDATA nic;
	// hook to get low level keyboard events
	static HHOOK hookproc = NULL;
	// tells the hook when to look for keyboard events
	static bool sessionLocked = false;
	// for updating the window
	static int cmdShow = NULL;
	// lock to prevent messaging bria too much if people spam the answer call button
	static bool answering = false;
	// registry method, used currently just for is starting at startup
	static Registry* registry = NULL;

	extern int InitWindow(LPCSTR windowClass, LPCSTR title, HINSTANCE instance, int nCmdShow);

	extern void InitIconData();

	// hwnd - handle of the window
	// uMsg - message your window has recieved.
	// wparam - extra params for the message
	// lParam - extra params for the message
	// https://msdn.microsoft.com/en-us/library/windows/desktop/ms633573(v=vs.85).aspx
	extern LRESULT CALLBACK WndProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

	// a callback used with SetWindowsHookEx function. the system calls this function 
	// everytime a new keyboard input event is about to be posted into the thread input queue.
	// nCode - A code the hook procedure uses to determine how to process the message.
	// wParam - The identifier of the keyboard message.This parameter can be one of the following messages : WM_KEYDOWN, WM_KEYUP, WM_SYSKEYDOWN, or WM_SYSKEYUP.
	// lParam - A pointer to a KBDLLHOOKSTRUCT structure.
	// https://msdn.microsoft.com/en-us/library/windows/desktop/ms644985%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
	extern LRESULT CALLBACK LowLevelKeyboardProc(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam);

	// this method exists for audioendpoint to call to tell the window that the volume changed, so it wants to answer the call
	extern void AudioEndpointCallback();

	// display the window and update it
	extern void Show();
	extern void Restore();
	extern void Hide();

	// returns true if we successfully registered the keyboard hook.
	// if not, quit, something is quite wrong and there is no point 
	// in this program without it.
	extern bool RegisterKeyboardHook();

	// Worker thread for contacting Bria phone.
	// Establishing if there is a phone call
	// if so, telling bria to answer it.
	extern void Worker(LPARAM lParam);

	// unregister hooks and any other cleanup if nessesary
	extern void Cleanup();

	// opens a pipe and finds if there is a ringing call, if there is, answer it.
	extern void AnswerCall();
}