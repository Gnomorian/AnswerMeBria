#include "Tray.h"

int Tray::Initialize(HINSTANCE thisInstance, int cmdShow) {
	WM_TASKBAR = 0;
	className = "AnswerMeBria";
	WM_TASKBAR = RegisterWindowMessageA("Taskbar Created");
	wincl.hInstance = thisInstance;
	wincl.lpszClassName = className.c_str();
	wincl.lpfnWndProc = Tray::WindowProcedure;      /* This function is called by windows */
	wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
	wincl.cbSize = sizeof(WNDCLASSEX);

	/*Use default icon and mouse-pointer*/
	wincl.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(ICO1));
	wincl.hIconSm = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(ICO1));
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wincl.lpszMenuName = NULL;                 /* No menu */
	wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
	wincl.cbWndExtra = 0;                      /* structure or the window instance */
	wincl.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(255, 255, 255)));
	/* Register the window class, and if it fails quit the program */
	if (!RegisterClassEx(&wincl))
		return 0;

	/* The class is registered, let's create the program*/
	Hwnd = CreateWindowEx(
		0,                   /* Extended possibilites for variation */
		className.c_str(),         /* Classname */
		className.c_str(),       /* Title Text */
		WS_OVERLAPPEDWINDOW, /* default window */
		CW_USEDEFAULT,       /* Windows decides the position */
		CW_USEDEFAULT,       /* where the window ends up on the screen */
		544,                 /* The programs width */
		375,                 /* and height in pixels */
		HWND_DESKTOP,        /* The window is a child-window to desktop */
		NULL,                /* No menu */
		thisInstance,       /* Program Instance handler */
		NULL                 /* No Window Creation data */
	);

	/*Initialize NOTIFYICONDATA structure only once*/
	InitNotifyIconData();
	/* Make the window visible on the screen */
	ShowWindow(Hwnd, cmdShow);
}

LRESULT CALLBACK Tray::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_TASKBAR && !IsWindowVisible(Hwnd)) {
		minimize();
		return 0;
	}
	switch (message) {
	case WM_ACTIVATE:
		Shell_NotifyIcon(NIM_ADD, &notifyIconData);
		break;
	case WM_CREATE:
		ShowWindow(Hwnd, SW_HIDE);
		Hmenu = CreatePopupMenu();
		AppendMenu(Hmenu, MF_STRING, ID_TRAY_EXIT, TEXT("Exit the demo"));
		break;
	case WM_SYSCOMMAND:
		// the first 4 bits are windows specific, app must combine 0xFFF0 with result to get the correct value
		switch (wParam & 0xFFF0) {
		case SC_MINIMIZE:
		case SC_CLOSE:
			minimize();
			return 0;
			break;
		}
		break;
		// our user defined WM_SYSICON message.
	case WM_SYSICON:
		switch (wParam) {
		case ID_TRAY_APP_ICON:
			SetForegroundWindow(Hwnd);
			break;
		}
		if (lParam == WM_LBUTTONUP) {
			restore();
		}
		else if (lParam == WM_RBUTTONDOWN) {
			// get current mouse position
			POINT curPoint;
			GetCursorPos(&curPoint);
			SetForegroundWindow(Hwnd);
			// TrackPopupMenu blocks the app until TrackPopupMenu returns
			UINT clicked = TrackPopupMenu(Hmenu, TPM_RETURNCMD | TPM_NONOTIFY, curPoint.x, curPoint.y, 0, hwnd, NULL);
			SendMessage(hwnd, WM_NULL, 0, 0); // send benign message to window to make sure the menu goes away.
			if (clicked == ID_TRAY_EXIT) {
				// quit the application
				Shell_NotifyIcon(NIM_DELETE, &notifyIconData);
				PostQuitMessage(0);
			}
		}
		break;

		// interrupt the hittest message..
	case WM_NCHITTEST:
	{
		UINT hittest = DefWindowProc(hwnd, WM_NCHITTEST, wParam, lParam);
		if (hittest == HTCLIENT)
			return HTCAPTION;
		else
			return hittest;
	}
	case WM_CLOSE:
		minimize();
		return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

void Tray::minimize() {
	ShowWindow(Hwnd, SW_HIDE);
}

void Tray::restore() {
	ShowWindow(Hwnd, SW_SHOW);
}

void Tray::InitNotifyIconData() {
	memset(&notifyIconData, 0, sizeof(NOTIFYICONDATA));

	notifyIconData.cbSize = sizeof(NOTIFYICONDATA);
	notifyIconData.hWnd = Hwnd;
	notifyIconData.uID = ID_TRAY_APP_ICON;
	notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	notifyIconData.uCallbackMessage = WM_SYSICON; // setup our invented windows message
	notifyIconData.hIcon = (HICON)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(ICO1));
	strncpy(notifyIconData.szTip, szTIP, sizeof(szTIP));
}