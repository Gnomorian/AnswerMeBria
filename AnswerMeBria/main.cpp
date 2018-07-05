#include "common.h"

int MessageLoop() {
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

void socketTest() {
	Socket socket("http://www.w3.org:80");
	//socket.Connect();
	socket.SendRequest();
	socket.Cleanup();
}

// entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	Log::InitLogging();

	socketTest();

	// setup the main parts of the window and handle errors
	int result = Window::InitWindow(TEXT("Answerme Bria!"), TEXT("Answerme Bria!"), hInstance);
	if (result != ERRORS::NOERR) {
		return result;
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