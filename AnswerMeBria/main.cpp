#include "common.h"

int MessageLoop() {
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

// entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
#ifdef DEBUG
	// initialize the console
	if (!Log::CreateConsole()) {
		MessageBox(NULL, TEXT("Could not create Console"), TEXT("Error"), NULL);
	}
#endif // DEBUG

	// setup the main parts of the window
	switch (Window::InitWindow(TEXT("Wil's Window"), TEXT("Win32 Guided Tour Application"), hInstance)) {
		case ERRORS::CREATE_WINDOW:
			MessageBox(NULL, TEXT("CreateWindow Failed"), TEXT("ERROR"), NULL);
			return ERRORS::CREATE_WINDOW;
		case ERRORS::REGISTER_CLASS_EX:
			MessageBox(NULL, TEXT("Failed to Register Window Class"), TEXT("ERROR"), NULL);
			return ERRORS::REGISTER_CLASS_EX;
		case ERRORS::REGISTER_SESSION_NOTIFICATION:
			MessageBox(NULL, TEXT("Failed to reigster Session Notification"), TEXT("ERROR"), NULL);
			return ERRORS::REGISTER_SESSION_NOTIFICATION;
	}
	Window::Show(nCmdShow);

	if (!Window::RegisterKeyboardHook())
		return ERRORS::KEYBOARD_LL_HOOK;
	
	// main loop - wait until window is closed
	int rtrn = MessageLoop();

	Window::Cleanup();
	Log::Cleanup();

	return rtrn;
}