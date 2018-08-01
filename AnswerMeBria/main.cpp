#include "common.h"

#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <Functiondiscoverykeys_devpkey.h>

#include "AudioEndpointVolumeCallback.h"
#include "AudioDeviceManager.h"

#include "Tray.h"

// the msg loop for the window
int MessageLoop() {
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		// window events
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		//Tray::Update();
	}
	return (int)msg.wParam;
}

// entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Log::InitLogging();

	// setup the main parts of the window and handle errors
	int result = Window::InitWindow(TEXT("Answerme Bria!"), TEXT("Answerme Bria!"), hInstance, nCmdShow);
	if (result != ERRORS::NOERR) {
		return result;
	}

	Window::Show();

	//if (!Window::RegisterKeyboardHook())
	//	return ERRORS::KEYBOARD_LL_HOOK;

	///////////////////////////////////////
	
	AudioEndpointVolumeCallback endpoint(&Window::AudioEndpointCallback);
	AudioDeviceManager manager;
	manager.InitializeCallback(&endpoint);

	// main loop - wait until window is closed
	int rtrn = MessageLoop();

	Window::Cleanup();
	Log::Cleanup();

	return rtrn;
}