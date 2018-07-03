#include "window.h"

#include <thread>

int Window::InitWindow(LPCSTR pWindowClass, LPCSTR pTitle, HINSTANCE instance)
{
	windowClass = pWindowClass;
	title = pTitle;

	// this defines information about my window such as its colour, name and most importantly for this application
	// the pointer to the windows process callback function so i can process windows messages.
	// https://msdn.microsoft.com/library/windows/desktop/ms633577
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc; // pointer to my msg callback function
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = instance;
	wcex.hIcon = LoadIcon(instance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = windowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	// register the WNDCLASSEX
	if (!RegisterClassEx(&wcex)) {
		MessageBox(NULL, TEXT("Call to RegisterClassEx failed"), TEXT("Error"), NULL);
		return ERRORS::REGISTER_CLASS_EX;
	}

	// create the window
	hwnd = CreateWindow(
		windowClass,			// the name of the application
		title,					// the text that apears in the title bar
		WS_OVERLAPPEDWINDOW,	// the type of window to create
		CW_USEDEFAULT,			// initial position x
		CW_USEDEFAULT,			// initial position y
		380,					// initial width
		100,					// initial height
		NULL,					// the parent of the window
		NULL,					// does this application have a menu bar
		instance,				// the first parameter from WinMain
		NULL					// a pointer to a CREATESTRUCT structure
	);
	if (!hwnd) {
		MessageBox(NULL, TEXT("Call to CreateWindow failed"), TEXT("Error"), NULL);
		return ERRORS::CREATE_WINDOW;
	}

	// register notification for lock screen
	if (!WTSRegisterSessionNotification(hwnd, NOTIFY_FOR_ALL_SESSIONS)) {
		MessageBox(NULL, TEXT("Could not Subscribe to SessionNotifications"), TEXT("Error"), NULL);
		return ERRORS::REGISTER_SESSION_NOTIFICATION;
	}
	return ERRORS::NOERR;
}

LRESULT CALLBACK Window::WndProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hdc; // a datastructure used to communicate with the graphics subsystem
	TCHAR line1[] = TEXT("While this window is open and your screen is locked");
	TCHAR line2[] = TEXT("If bria is ringing a key press on your lock screen will");
	TCHAR line3[] = TEXT("answer the phone for you");

	switch (uMsg) {
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		TextOut(hdc, 5, 5, line1, _tcslen(line1));
		TextOut(hdc, 5, 22, line2, _tcslen(line2));
		TextOut(hdc, 5, 39, line3, _tcslen(line3));
		// end application-specific layout section.
		EndPaint(hwnd, &ps);
		break;
	case WM_WTSSESSION_CHANGE:
		if (wParam == WTS_SESSION_UNLOCK) { // the session was unlocked, let the hook know.
			Log::LogMessage("WTS_SESSION_UNLOCK", Log::LOGLEVEL::DEBUGG);
			sessionLocked = false;
			break;
		}
		if (wParam == WTS_SESSION_LOCK) { // the session was locked, let the hook know.
			Log::LogMessage("WTS_SESSION_LOCK", Log::LOGLEVEL::DEBUGG);
			sessionLocked = true;
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
		break;
	}
	return 0;
}

LRESULT CALLBACK Window::LowLevelKeyboardProc(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam) {
	// if nCode is less that 0, this message isnt for us, pass it on.
	if (nCode < 0) {
		return CallNextHookEx(NULL, nCode, wParam, lParam);
	}

	// pass processing to a detached thread so we can pass 
	// to the next hook as quickly as possible.
	if (wParam == WM_KEYDOWN && sessionLocked) {
		std::thread thread(&Window::Worker, lParam);
		thread.detach();
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void Window::Show(int CmdShow) {
	ShowWindow(hwnd, CmdShow);
	UpdateWindow(hwnd);
}

bool Window::RegisterKeyboardHook() {
	hookproc = SetWindowsHookEx(WH_KEYBOARD_LL, &Window::LowLevelKeyboardProc, NULL, NULL);
	if (hookproc == NULL) {
		DWORD errormsg = GetLastError();
		MessageBox(NULL, TEXT("could not create hook to SetWindowsHookEx"), TEXT("Error"), NULL);
		return false;
	}
	return true;
}

void Window::Worker(LPARAM lParam) {
	std::string message("Key Down: ");
	KBDLLHOOKSTRUCT* keyStruct = (KBDLLHOOKSTRUCT*)lParam;
	message += keyStruct->vkCode;
	Log::LogMessage(message);
}

void Window::Cleanup() {
	UnhookWindowsHookEx(hookproc);
}