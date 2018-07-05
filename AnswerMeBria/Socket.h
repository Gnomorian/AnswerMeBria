#pragma once

#include "common.h"
#include <vector>
#include <winhttp.h>

class Socket
{
public:
	Socket(const std::string& serveraddress);
	~Socket();
	bool Connect();
	void Cleanup();
	void SendRequest();

private:
	// handle to the 
	WEB_SOCKET_HANDLE socket_handle;

	// vector of headers to send to the server
	std::vector<WEB_SOCKET_HTTP_HEADER>headers;
};

