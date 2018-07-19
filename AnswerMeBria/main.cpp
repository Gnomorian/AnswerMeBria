#include "common.h"

// temp function to tag on websockets to the existing project

Pipe* p = NULL;

void testPipes() {
	std::string callerId;
	p = new Pipe((TCHAR*)TEXT("\\\\.\\pipe\\apipipe"));
	p->InitializePipe();
	//p->PipeMessage(p->ConstructMessage("bringToFront", ""));
	// get call status
	p->PipeMessage(p->ConstructMessage("status", "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n<status><type>call</type></status>"));
	std::string message;
	do {// keep checking the pipe until you get the proper responce
		message = p->RecieveMessage();
	} while (message.find("<status type=\"call\">") == std::string::npos);
	// answer call
	std::cout << message << std::endl;
}

int MessageLoop() {
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		// window events
		TranslateMessage(&msg);
		DispatchMessage(&msg);
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

	testPipes();

	Window::Show(nCmdShow);

	if (!Window::RegisterKeyboardHook())
		return ERRORS::KEYBOARD_LL_HOOK;

	// main loop - wait until window is closed
	int rtrn = MessageLoop();

	Window::Cleanup();
	delete p;
	Log::Cleanup();

	return rtrn;
}