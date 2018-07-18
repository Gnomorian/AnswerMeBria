#include "common.h"

// temp function to tag on websockets to the existing project

WebSocket* ws = NULL;

void testWebsocket() {
	ws = new WebSocket(L"demos.kaazing.com", 80, L"/echo");

	ws->InitializeWebSocket();
	ws->SendMessage(L"This is a message", 17);
	ws->SendMessage(L"This is another message", 23);
}

int MessageLoop() {
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		// window events
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		// websocket recieve
		ws->RecieveMessage();
	}
	return (int)msg.wParam;
}

// entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
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
	ws->EndConnection();
	delete ws;
	Log::Cleanup();


	return rtrn;
}