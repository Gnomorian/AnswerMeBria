#include "Common.h"


Socket::Socket(const std::string& serveraddress) {

	// create the header structure
	// 1 header of "Host:url" is needed at a minimum
	WEB_SOCKET_HTTP_HEADER* host = new WEB_SOCKET_HTTP_HEADER;
	PCHAR phost = (char*)"Host";
	PCHAR pValue = (char*)"http://www.w3.org:80";
	host->pcName = phost;
	host->ulNameLength = sizeof(phost);
	host->pcValue = pValue;
	host->ulValueLength = sizeof(pValue);
	headers.push_back(*host);
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
	if (WebSocketBeginClientHandshake(socket_handle, NULL, 0, NULL, 0, &headers[0], 1, &responceHeaders, &responceHeadderCount) != S_OK) {
		return -1;
	}
	// name nameLength, value valuelength
	for (int i = 0; i < responceHeadderCount; i++) {
		std::cout << responceHeaders[i].pcName << "x" << responceHeaders[i].ulNameLength << ":" << responceHeaders[i].pcValue << "x" << responceHeaders[i].ulValueLength << std::endl;
	}
	

	ULONG selected_subprotocol;
	ULONG selected_extensions;
	ULONG selected_extensions_count;
	HRESULT result = WebSocketEndClientHandshake(socket_handle, responceHeaders, responceHeadderCount, &selected_extensions, &selected_extensions_count, &selected_subprotocol);
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
		Log::LogMessage("WebSocketEndClientHandshake returned unsupported return code: "+result, Log::LOGLEVEL::CRITICAL);
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


	//WinHttpOpenRequest();

	//WinHttpSendRequest()
}