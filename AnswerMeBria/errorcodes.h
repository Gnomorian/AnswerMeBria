#pragma once


// error codes
enum ERRORS {
	NOERR,							// no error, return 0
	REGISTER_CLASS_EX,				// failed to register the windows LPCSTR windowClass
	KEYBOARD_LL_HOOK,				// failed to hook into the low level keyboard event HHOOK is NULL.
	CREATE_WINDOW,					// failed to create the window, HWND is NULL.
	REGISTER_SESSION_NOTIFICATION	// failed to register for session notifications.
};