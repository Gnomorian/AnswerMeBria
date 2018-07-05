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

	// utiliy method to create headers
	WEB_SOCKET_HTTP_HEADER* CreateHeader(const std::string &name, const std::string &value);

	// utility method to add requested headers without duplicates
	void AddHeaders_NoDuplicates(WEB_SOCKET_HTTP_HEADER* duplicate_headers, ULONG count);
};

