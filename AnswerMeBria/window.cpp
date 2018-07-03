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
		MessageBox(NULL, TEXT("Call to RegisterClassEx failed"), TEXT("Win32 guided tour"), NULL);
		return ERRORS::REGISTER_CLASS_EX;
	}

	// create the window
	hwnd = CreateWindow(
		windowClass,			// the name of the application
		title,					// the text that apears in the title bar
		WS_OVERLAPPEDWINDOW,	// the type of window to create
		CW_USEDEFAULT,			// initial position x
		CW_USEDEFAULT,			// initial position y
		500,					// initial width
		100,					// initial height
		NULL,					// the parent of the window
		NULL,					// does this application have a menu bar
		instance,				// the first parameter from WinMain
		NULL					// a pointer to a CREATESTRUCT structure
	);
	if (!hwnd) {
		MessageBox(NULL, TEXT("Call to CreateWindow failed"), TEXT("Win32 Guided Tour"), NULL);
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
	TCHAR greeting[] = TEXT("Hello, World");

	switch (uMsg) {
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		// here your application is layed out
		// for this introcuction we just print out hellow world
		// in our top left corner
		TextOut(hdc, 5, 5, greeting, _tcslen(greeting));
		// end application-specific layout section.
		EndPaint(hwnd, &ps);
		break;
	case WM_WTSSESSION_CHANGE:
		if (wParam == WTS_SESSIONSTATE_UNLOCK) {
			Log::LogMessage("WTS_SESSION_UNLOCK");
			sessionLocked = false;
			break;
		}
		if (wParam == WTS_SESSION_LOCK) {
			Log::LogMessage("WTS_SESSION_LOCK");
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
	// if nCode is less than 1, must pass back and do no more processing.
	// if nCode == HC_ACTION wParam and lParam has information for you
	static int num = 0;
	static std::string msg;
	if (nCode < 0) {
		return CallNextHookEx(NULL, nCode, wParam, lParam);
	}

	switch (wParam) {
	case WM_KEYDOWN:
	{
		if (sessionLocked) {
			std::thread thread(&Window::Worker, lParam);
			thread.detach();
		}
		break;
	}
	case WM_KEYUP:
		break;
	case WM_SYSKEYDOWN:
		break;
	case WM_SYSKEYUP:
		break;

	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void Window::Show(int CmdShow) {
	ShowWindow(hwnd, CmdShow);
	UpdateWindow(hwnd);
}

bool Window::RegisterKeyboardHook() {
	// register keyboard hook
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