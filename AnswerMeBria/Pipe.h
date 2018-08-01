#pragma once

//#include "common.h"

#include <tchar.h>
#include <string>
#include <Windows.h>/**/

class Pipe
{
public:
	Pipe(TCHAR* pipeName);
	Pipe();
	~Pipe();
	int PipeMessage(std::string content);
	std::string RecieveMessage();
	int InitializePipe();
	int InitializePipe(TCHAR* pipeName);
	static std::string ConstructMessage(std::string type, std::string content);

private:
	HANDLE hPipe;
	TCHAR  chBuf[512];
	BOOL   fSuccess;
	DWORD  cbRead, cbToWrite, cbWritten, dwMode;
	LPTSTR lpszPipename;
};

