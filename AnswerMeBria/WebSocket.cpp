#include "WebSocket.h"

WebSocket::WebSocket(LPCWSTR server, INTERNET_PORT port, LPCWSTR path) {
	_serverName = server;
	_port = port;
	_path = path;
	_errmsg = ERROR_SUCCESS;
}

WebSocket::~WebSocket() {
	if (_requestHandle != NULL)
	{
		WinHttpCloseHandle(_requestHandle);
		_requestHandle = NULL;
	}

	if (_websocketHandle != NULL)
	{
		WinHttpCloseHandle(_websocketHandle);
		_websocketHandle = NULL;
	}

	if (_connectionHandle != NULL)
	{
		WinHttpCloseHandle(_connectionHandle);
		_connectionHandle = NULL;
	}

	if (_sessionHandle != NULL)
	{
		WinHttpCloseHandle(_sessionHandle);
		_sessionHandle = NULL;
	}
}

int WebSocket::InitializeWebSocket() {
	BOOL status = FALSE;

	//
	// Create session, connection and request handles.
	//
	Log::LogMessage("Creating Session Handle");
	_sessionHandle = WinHttpOpen(L"AnswerMeBria", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, NULL, NULL, 0);
	if (_sessionHandle == NULL)
	{
		_errmsg = GetLastError();
		return -1;
	}

	Log::LogMessage("Creating Connection Handle");
	_connectionHandle = WinHttpConnect(_sessionHandle, _serverName, _port, 0);
	if (_connectionHandle == NULL)
	{
		_errmsg = GetLastError();
		return -1;
	}

	Log::LogMessage("Creating Request Handle");
	_requestHandle = WinHttpOpenRequest(_connectionHandle, L"GET", _path, NULL, NULL, NULL,	0);
	if (_requestHandle == NULL)
	{
		_errmsg = GetLastError();
		return -1;
	}

	//
	// Request protocol upgrade from http to websocket.
	//
	Log::LogMessage("Creating Upgrade Header");
#pragma prefast(suppress:6387, "WINHTTP_OPTION_UPGRADE_TO_WEB_SOCKET does not take any arguments.")
	status = WinHttpSetOption(_requestHandle, WINHTTP_OPTION_UPGRADE_TO_WEB_SOCKET, NULL, 0);
	if (!status)
	{
		_errmsg = GetLastError();
		return -1;
	}

	//
	// Perform websocket handshake by sending a request and receiving server's response.
	// Application may specify additional headers if needed.
	//

	Log::LogMessage("Upgrading to Websocket");
	status = WinHttpSendRequest(_requestHandle,	WINHTTP_NO_ADDITIONAL_HEADERS, 0, NULL, 0, 0, 0);
	if (!status)
	{
		_errmsg = GetLastError();
		return -1;
	}

	Log::LogMessage("Waiting for Upgrade Responce");
	status = WinHttpReceiveResponse(_requestHandle, 0);
	if (!status)
	{
		_errmsg = GetLastError();
		return -1;
	}

	//
	// Application should check what is the HTTP status code returned by the server and behave accordingly.
	// WinHttpWebSocketCompleteUpgrade will fail if the HTTP status code is different than 101.
	//
	Log::LogMessage("Websocket Upgrade Completed");
	_websocketHandle = WinHttpWebSocketCompleteUpgrade(_requestHandle, NULL);
	if (_websocketHandle == NULL)
	{
		_errmsg = GetLastError();
		return -1;
	}

	//
	// The request handle is not needed anymore. From now on we will use the websocket handle.
	//

	WinHttpCloseHandle(_requestHandle);
	_requestHandle = NULL;

	Log::LogMessage("Succesfully upgraded to websocket protocol");
}

// gracefully shut down the connection to the server
int WebSocket::EndConnection() {

	_errmsg = WinHttpWebSocketClose(_websocketHandle, WINHTTP_WEB_SOCKET_SUCCESS_CLOSE_STATUS, NULL, 0);
	if (_errmsg != ERROR_SUCCESS)
	{
		return -1;
	}

	//
	// Check close status returned by the server.
	//

	_errmsg = WinHttpWebSocketQueryCloseStatus(_websocketHandle, &usStatus, _rgbCloseReasonBuffer, ARRAYSIZE(_rgbCloseReasonBuffer), &_dwCloseReasonLength);
	if (_errmsg != ERROR_SUCCESS)
	{
		return -1;
	}

	wprintf(L"The server closed the connection with status code: '%d' and reason: '%.*S'\n", (int)usStatus,	_dwCloseReasonLength, _rgbCloseReasonBuffer);
}

int WebSocket::SendMessage(const WCHAR* message, int size) {
	const DWORD messageLength = size * sizeof(WCHAR);
	//
	// Send and receive data on the websocket protocol.
	//

	_errmsg = WinHttpWebSocketSend(_websocketHandle, WINHTTP_WEB_SOCKET_BINARY_MESSAGE_BUFFER_TYPE, (PVOID)message, messageLength);
	if (_errmsg != ERROR_SUCCESS)
	{
		return -1;
	}

	wprintf(L"Sent message to the server: '%s'\n", message);

	
}

int WebSocket::RecieveMessage() {	
	do
	{
		if (_dwBufferLength == 0)
		{
			_errmsg = ERROR_NOT_ENOUGH_MEMORY;
			return -1;
		}

		_errmsg = WinHttpWebSocketReceive(_websocketHandle, _pbCurrentBufferPointer, _dwBufferLength, &_dwBytesTransferred, &eBufferType);
		if (_errmsg != ERROR_SUCCESS)
		{
			return -1;
		}
		//
		// If we receive just part of the message restart the receive operation.
		//
		
		_pbCurrentBufferPointer += _dwBytesTransferred;
		_dwBufferLength -= _dwBytesTransferred;
	} while (eBufferType == WINHTTP_WEB_SOCKET_BINARY_FRAGMENT_BUFFER_TYPE);

	//
	// We expected server just to echo single binary message.
	//

	if (eBufferType != WINHTTP_WEB_SOCKET_BINARY_MESSAGE_BUFFER_TYPE)
	{
		wprintf(L"Unexpected buffer type\n");
		_errmsg = ERROR_INVALID_PARAMETER;
		return -1;
	}

	wprintf(L"Received message from the server: '%.*s'\n", _dwBufferLength, (WCHAR*)_rgbBuffer);
	ClearBuffer();
}

// the recieve loop sits on its own thread, seem that WinHttpWebSocketReceive 
// doesn't have a timeout, so it cant sit in main's MessageLoop like i intended.
void WebSocket::RecieveLoop(bool running) {

}

void WebSocket::ClearBuffer() {
	_pbCurrentBufferPointer = _rgbBuffer;
	_dwBufferLength = ARRAYSIZE(_rgbBuffer);
}

void WebSocket::WriteError() {
	std::cout << _errmsg << std::endl;
}