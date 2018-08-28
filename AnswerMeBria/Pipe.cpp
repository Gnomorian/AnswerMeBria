#include "Pipe.h"

#include "Logger.h"

Pipe::Pipe() {}

Pipe::Pipe(TCHAR* pipeName) : fSuccess(false)
{
	lpszPipename = pipeName;
}

Pipe::~Pipe()
{
	CloseHandle(hPipe);

}

int Pipe::PipeMessage(std::string message) {
	// Send a message to the pipe server. 

	cbToWrite = (message.size() + 1) * sizeof(TCHAR);
	Log::LogMessageF("Sending %d byte message: \"%s\"\n", Log::LOGLEVEL::DEBUGG, cbToWrite, message.c_str());

	fSuccess = WriteFile(
		hPipe,                  // pipe handle 
		message.c_str(),        // message 
		cbToWrite,              // message length 
		&cbWritten,             // bytes written 
		NULL);                  // not overlapped 

	if (!fSuccess)
	{
		Log::LogMessageF("WriteFile to pipe failed. GLE=%d\n", Log::LOGLEVEL::CRITICAL, GetLastError());
		return -1;
	}

	Log::LogMessage("\nMessage sent to server, receiving reply as follows:\n");
}

// gets a message from the pipe
std::string Pipe::RecieveMessage() {
	std::string message;
	do// loop while the buffer is smaller than the data 
	{
		// Read from the pipe. 

		fSuccess = ReadFile(
			hPipe,    // pipe handle 
			chBuf,    // buffer to receive reply 
			512 * sizeof(TCHAR),  // size of buffer 
			&cbRead,  // number of bytes read 
			NULL);    // not overlapped 

		// add data to the string
		for (int i = 0; i < cbRead; i++) {
			message += chBuf[i];
		}

		if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
			break;

	} while (!fSuccess);  // repeat loop if ERROR_MORE_DATA 

	if (!fSuccess)
	{
		Log::LogMessageF("ReadFile from pipe failed. (%d)", Log::LOGLEVEL::CRITICAL, GetLastError());
		return "";
	}
	else {
		return message;
	}
}

int Pipe::InitializePipe(TCHAR* pipeName) {
	lpszPipename = pipeName;
	InitializePipe();
	return 0;
}

int Pipe::InitializePipe() {
	// Try to open a named pipe; wait for it, if necessary. 

	while (1)
	{
		hPipe = CreateFile(
			lpszPipename,   // pipe name 
			GENERIC_READ |  // read and write access 
			GENERIC_WRITE,
			0,              // no sharing 
			NULL,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe 
			0,              // default attributes 
			NULL);          // no template file 

							// Break if the pipe handle is valid. 

		if (hPipe != INVALID_HANDLE_VALUE)
			break;

		// Exit if an error other than ERROR_PIPE_BUSY occurs. 

		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			Log::LogMessageF("Could not open pipe. (%d)", Log::LOGLEVEL::CRITICAL, GetLastError());

			return -1;
		}

		// All pipe instances are busy, so wait for 20 seconds. 

		if (!WaitNamedPipe(lpszPipename, 20000))
		{
			Log::LogMessage("Could not open pipe: 20 second wait timed out.");
			return -1;
		}
	}

	// The pipe connected; change to message-read mode. 

	dwMode = PIPE_READMODE_MESSAGE;
	fSuccess = SetNamedPipeHandleState(
		hPipe,    // pipe handle 
		&dwMode,  // new pipe mode 
		NULL,     // don't set maximum bytes 
		NULL);    // don't set maximum time 
	if (!fSuccess)
	{
		Log::LogMessageF("SetNamedPipeHandleState failed. (%d)", Log::LOGLEVEL::CRITICAL, GetLastError());
		return -1;
	}
}

std::string Pipe::ConstructMessage(std::string type, std::string content) {
	std::string message("GET /");
	message += type;
	message += "\r\nUser-Agent: AnswerMeBria\r\nContent-Type: application/xml\r\nContent-Length: ";
	if (content == "") {
		message += "0";
	}
	else {
		message += std::to_string(message.size());
		message += "\r\n";
		message += content;
	}
	return message;
}