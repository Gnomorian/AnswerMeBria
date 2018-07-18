#pragma once

#include "common.h"

class WebSocket
{
public:
	WebSocket(LPCWSTR server, INTERNET_PORT port, LPCWSTR path);
	~WebSocket();
	int InitializeWebSocket();
	int EndConnection();
	int SendMessage(const WCHAR* message, int size);
	int RecieveMessage();
private:
	HINTERNET _sessionHandle;
	HINTERNET _connectionHandle;
	HINTERNET _requestHandle;
	HINTERNET _websocketHandle;

	LPCWSTR _serverName;
	INTERNET_PORT _port;
	const WCHAR* _path;
	USHORT usStatus;
	DWORD _dwCloseReasonLength = 0;
	BYTE _rgbCloseReasonBuffer[123];
	BYTE _rgbBuffer[1024];
	BYTE *_pbCurrentBufferPointer = _rgbBuffer;
	DWORD _dwBufferLength = ARRAYSIZE(_rgbBuffer);
	DWORD _dwBytesTransferred = 0;
	DWORD _errmsg;
	WINHTTP_WEB_SOCKET_BUFFER_TYPE eBufferType;

	void ClearBuffer();
};

