#pragma once

#include "common.h"

class Pipe
{
public:
	Pipe(TCHAR* pipeName);
	~Pipe();
	int PipeMessage(std::string content);
	std::string RecieveMessage();
	int InitializePipe();
	std::string ConstructMessage(std::string type, std::string content);

private:
	HANDLE hPipe;
	TCHAR  chBuf[512];
	BOOL   fSuccess;
	DWORD  cbRead, cbToWrite, cbWritten, dwMode;
	LPTSTR lpszPipename;
};

