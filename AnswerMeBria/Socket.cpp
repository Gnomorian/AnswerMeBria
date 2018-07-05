#include "Common.h"

#define _SCL_SECURE_NO_WARNINGS

bool operator==(WEB_SOCKET_HTTP_HEADER a, WEB_SOCKET_HTTP_HEADER b) {
	if (a.ulNameLength != b.ulNameLength)
		return false;

	for (int i = 0; i < a.ulNameLength; i++) {
		if (a.pcName[i] != b.pcName[i])
			return false;
	}
	return true;
}

Socket::Socket(const std::string& serveraddress) {
	// create the header structure
	WEB_SOCKET_HTTP_HEADER* host = CreateHeader("Host", serveraddress);
	headers.push_back(*host);
	// lets try add the other headers shown in https://en.wikipedia.org/wiki/WebSocket
	WEB_SOCKET_HTTP_HEADER* upgrade = CreateHeader("Upgrade", "websocket");
	WEB_SOCKET_HTTP_HEADER* websec_key = CreateHeader("Sec-WebSocket-Key", "54dz8f4sdf5784sf51x)");
	WEB_SOCKET_HTTP_HEADER* websec_version = CreateHeader("Sec-WebSocket-Version", "13");
	WEB_SOCKET_HTTP_HEADER* origin = CreateHeader("Origin", "wss://echo.websocket.org");
	WEB_SOCKET_HTTP_HEADER* websec_protocol = CreateHeader("Sec-WebSocket-Protocol", "chat");
	headers.push_back(*upgrade);
	headers.push_back(*websec_key);
	headers.push_back(*websec_version);
	headers.push_back(*origin);
}

Socket::~Socket() {}

bool Socket::Connect() {
	if (WebSocketCreateClientHandle(NULL, 0, &socket_handle) != S_OK) {
		Log::LogMessage("WebSocketCreateClientHandle Failed", Log::LOGLEVEL::CRITICAL);
		return 1;
	}

	// if handshake is successful, these will be populated
	WEB_SOCKET_HTTP_HEADER* responceHeaders = nullptr;
	ULONG responceHeadderCount;
	if (WebSocketBeginClientHandshake(socket_handle, NULL, 0, NULL, 0, &headers[0],  headers.size(), &responceHeaders, &responceHeadderCount) != S_OK) {
		return -1;
	}

	AddHeaders_NoDuplicates(responceHeaders, responceHeadderCount);

	if (WebSocketCreateClientHandle(NULL, 0, &socket_handle) != S_OK) {
		Log::LogMessage("WebSocketCreateClientHandle Failed", Log::LOGLEVEL::CRITICAL);
		return 1;
	}

/*	// writes the headers in headers[] after AddHeaders_NoDuplicates
	for (int i = 0; i < headers.size(); i++) {
		printf("%.*s:%.*s\n", headers[i].ulNameLength, headers[i].pcName, headers[i].ulValueLength, headers[i].pcValue);
	}/**/
	
	ULONG selected_subprotocol;
	ULONG selected_extensions;
	ULONG selected_extensions_count;
	HRESULT result = WebSocketEndClientHandshake(socket_handle, &headers[0], headers.size(), &selected_extensions, &selected_extensions_count, &selected_subprotocol);
	switch (result) {
	case S_OK:
		return true;
	case E_INVALID_PROTOCOL_FORMAT:
		Log::LogMessage("Protocol data had an invalid format.", Log::LOGLEVEL::CRITICAL);
		break;
		// even though they are listed on msdn, these return codes dont seem to be defined.
		/*			case E_UNSUPPORTED_SUBPROTOCOL:
		Log::LogMessage("Protocol data had an invalid format.");
		break;/**/
		/*			case E_UNSUPPORTED_EXTENSION:
		Log::LogMessage("Protocol data had an invalid format.");
		break;/**/
	default:
		std::string msg = "WebSocketEndClientHandshake returned unsupported return code: ";
		msg += result;
		Log::LogMessage(msg, Log::LOGLEVEL::CRITICAL);
	}
}

void Socket::Cleanup() {
	WebSocketDeleteHandle(socket_handle);
}

void Socket::SendRequest() {
		// initializes, for an application, the use of WinHTTP functions and returns a WinHTTP-session handle.
		HINTERNET hSession = WinHttpOpen(L"A WinHTTP Example Program/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
		if (hSession == NULL) {
			Log::LogMessage("WinHttpOpen failed");
			return;
		}
		
		// specifies the initial target server of an HTTP request and returns an HINTERNET connection handle
		HINTERNET conn = WinHttpConnect(hSession, L"demos.kaazing.com", 443, 0);
		if (conn == NULL) {
			Log::LogMessage("WinHttpConnect failed");
			return;
		}

		// creates an HTTP request handle.
		LPCWSTR requestType = L"GET";
		LPCWSTR resource = L"echo";
		HINTERNET request = WinHttpOpenRequest(conn, requestType, resource, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
		if (request == NULL) {
			Log::LogMessage("WinHttpOpenRequest failed");
			return;
		}

		// sets an Internet option on the request handle so it is treated as a WebSocket request
		if (!WinHttpSetOption(request, WINHTTP_OPTION_UPGRADE_TO_WEB_SOCKET, NULL, 0)) {
			Log::LogMessage("WinHttpSetOption failed for WinHttpSendRequest");
			return;
		}/**/

		// sends the specified request to the HTTP server
		if (!WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, NULL, 0, 0, 0)) {
			Log::LogMessage("WinHttpSendRequest failed");
			return;
		}

		// completes a WebSocket handshake started by WinHttpSendRequest
		HINTERNET completeUpgrade = WinHttpWebSocketCompleteUpgrade(request, 0);
		if (completeUpgrade == NULL) {
			Log::LogMessage("WinHttpWebSocketCompleteUpgrade failed");
			std::cout << GetLastError() << std::endl;
			return;
		}
}


WEB_SOCKET_HTTP_HEADER* Socket::CreateHeader(const std::string &pName, const std::string &pValue) {
	WEB_SOCKET_HTTP_HEADER* header = new WEB_SOCKET_HTTP_HEADER;

	PCHAR name = new CHAR[pName.length()];
	pName.copy(name, pName.length());
	header->ulNameLength = pName.length();
	header->pcName = name;

	PCHAR value = new CHAR[pValue.length()];
	pValue.copy(value, pValue.length());
	header->pcValue = value;
	header->ulValueLength = pValue.length();

	return header;
}

void Socket::AddHeaders_NoDuplicates(WEB_SOCKET_HTTP_HEADER* duplicate_headers, ULONG count) {
	std::vector<WEB_SOCKET_HTTP_HEADER> temp;

	// replace headers in the vector with copies from the duplicates.
	for (int i = 0; i < headers.size(); i++) {
		for (int j = 0; j < count; j++) {
			if (headers[i] == duplicate_headers[j]) {
				headers[i] = duplicate_headers[j];
			}
		}
	}
	// add headers that are in duplicates and not in the original, to the original.
	for (int j = 0; j < count; j++) {
		for (int i = 0; i < headers.size(); i++) {
			if (headers[i] == duplicate_headers[j]) {
				goto skip_loop;
			}
		}
		headers.push_back(duplicate_headers[j]);
		skip_loop:;
	}
}

