#include "common.h"

// temp function to tag on websockets to the existing project
void testWebsocket() {
	WebSocket* ws = new WebSocket(L"demos.kaazing.com", 80, L"/echo");

	ws->InitializeWebSocket();
	ws->SendMessage(L"This is a message", 17);
	ws->RecieveMessage();
	ws->SendMessage(L"This is another message", 23);
	ws->RecieveMessage();
	char end;
	std::cin >> end;

	// epilog
	ws->EndConnection();
	delete ws;
}

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
	Log::InitLogging();

	// setup the main parts of the window and handle errors
	int result = Window::InitWindow(TEXT("Answerme Bria!"), TEXT("Answerme Bria!"), hInstance);
	if (result != ERRORS::NOERR) {
		return result;
	}

	testWebsocket();

	Window::Show(nCmdShow);

	if (!Window::RegisterKeyboardHook())
		return ERRORS::KEYBOARD_LL_HOOK;

	// main loop - wait until window is closed
	int rtrn = MessageLoop();

	Window::Cleanup();
	Log::Cleanup();

	return rtrn;
}