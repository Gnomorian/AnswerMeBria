#include "window.h"

#include <thread>
#include "resource.h"

int Window::InitWindow(LPCSTR pWindowClass, LPCSTR pTitle, HINSTANCE instance, int nCmdShow)
{
	windowClass = pWindowClass;
	title = pTitle;
	cmdShow = nCmdShow;

	WM_TASKBAR = RegisterWindowMessageA("Taskbar Created");

	// this defines information about my window such as its colour, name and most importantly for this application
	// the pointer to the windows process callback function so i can process windows messages.
	// https://msdn.microsoft.com/library/windows/desktop/ms633577
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
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

	InitIconData();

	return ERRORS::NOERR;
}

void Window::InitIconData() {
	memset(&nic, 0, sizeof(NOTIFYICONDATA));

	nic.cbSize = sizeof(NOTIFYICONDATA);
	nic.hWnd = hwnd;
	nic.uID = ID_TRAY_APP_ICON;
	nic.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nic.uCallbackMessage = WM_SYSICON; // setup our invented windows message
	nic.hIcon = (HICON)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(ICO1));
	strncpy(nic.szTip, title, sizeof(title));
}

LRESULT CALLBACK Window::WndProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hdc; // a datastructure used to communicate with the graphics subsystem
	TCHAR line1[] = TEXT("While this window is open and your screen is locked");
	TCHAR line2[] = TEXT("If bria is ringing a key press on your lock screen will");
	TCHAR line3[] = TEXT("answer the phone for you");

	switch (uMsg) {
	case WM_ACTIVATE:
		Shell_NotifyIcon(NIM_ADD, &nic);
		break;
	case WM_CREATE:
		Hmenu = CreatePopupMenu();
		AppendMenu(Hmenu, MF_STRING, ID_TRAY_EXIT, TEXT("Exit"));
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		TextOut(hdc, 5, 5, line1, _tcslen(line1));
		TextOut(hdc, 5, 22, line2, _tcslen(line2));
		TextOut(hdc, 5, 39, line3, _tcslen(line3));
		// end application-specific layout section.
		EndPaint(hwnd, &ps);
		break;
	case WM_WTSSESSION_CHANGE: // did the screen get locked?
		if (wParam == WTS_SESSION_UNLOCK) {
			Log::LogMessage("WTS_SESSION_UNLOCK", Log::LOGLEVEL::DEBUGG);
			sessionLocked = false;
			break;
		}
		if (wParam == WTS_SESSION_LOCK) {
			Log::LogMessage("WTS_SESSION_LOCK", Log::LOGLEVEL::DEBUGG);
			sessionLocked = true;
			break;
		}
		break;
	case WM_SYSICON:
		switch (wParam) {
		case ID_TRAY_APP_ICON:
			SetForegroundWindow(hwnd);
			break;
		}
		if (lParam == WM_LBUTTONUP) {
			Window::Restore();
		}
		else if (lParam == WM_RBUTTONDOWN) {
			// get current mouse position
			POINT curPoint;
			GetCursorPos(&curPoint);
			SetForegroundWindow(hwnd);
			// TrackPopupMenu blocks the app until TrackPopupMenu returns
			UINT clicked = TrackPopupMenu(Hmenu, TPM_RETURNCMD | TPM_NONOTIFY, curPoint.x, curPoint.y, 0, hwnd, NULL);
			SendMessage(hwnd, WM_NULL, 0, 0); // send benign message to window to make sure the menu goes away.
			if (clicked == ID_TRAY_EXIT) {
				// quit the application
				Shell_NotifyIcon(NIM_DELETE, &nic);
				PostQuitMessage(0);
			}
		}
		break;
	case WM_SYSCOMMAND:
		switch (wParam & 0xFFF0) {
		case SC_MINIMIZE:
		case SC_CLOSE:
			Window::Hide();
			return 0;
			break;
		}
		break;
	case WM_NCHITTEST:
	{
		UINT hittest = DefWindowProc(hwnd, WM_NCHITTEST, wParam, lParam);
		if (hittest == HTCLIENT)
			return HTCAPTION;
		else
			return hittest;
	}
	case WM_CLOSE:
		Window::Hide();
		return 0;
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

	if (wParam == WM_KEYDOWN && sessionLocked) {
		std::thread thread(&Window::Worker, lParam);
		thread.detach();
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void Window::Show() {
	ShowWindow(hwnd, cmdShow);
	UpdateWindow(hwnd);
	Window::Hide();
}

void Window::Restore() {
	ShowWindow(hwnd, SW_SHOW);
}

void Window::Hide() {
	ShowWindow(hwnd, SW_HIDE);
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
	AnswerCall();
}

void Window::Cleanup() {
	UnhookWindowsHookEx(hookproc);
}

void Window::AnswerCall() {
	// if one of these threads are already answering the call, quit
	if (answering)
		return;
	answering = true;
	// initialize pipe and api
	std::string callerId;
	Pipe *p = new Pipe((TCHAR*)TEXT("\\\\.\\pipe\\apipipe"));
	p->InitializePipe();
	BriaAPI *api = new BriaAPI();

	// get the current call status
	std::string message;
	int responce = api->GetCallStatus(message, *p);
	std::cout << message << std::endl;

	// get the caller id if there is one
	// answer the call if there is a caller id
	if (api->HasRingingCall(message)) {
		Log::LogMessage("There is a ringing call!");
		std::string callerId;
		api->GetCallerId(message, callerId);
		Log::LogMessage("Callerid is " + callerId);
		api->AnswerCall(*p, callerId, message);
		Log::LogMessage(message);
	}
	else {
		Log::LogMessage("No Active Call");
	}
	delete p;
	delete api;
	answering = false;
}

void Window::AudioEndpointCallback() {
	if (sessionLocked) {
		std::cout << "Session Locked and Audio Changed, Answeing Call" << std::endl;
		Window::AnswerCall();
	}
}