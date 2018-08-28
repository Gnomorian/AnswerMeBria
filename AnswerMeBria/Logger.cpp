#include "Logger.h"
#include <vector>

#define _CRT_SECURE_NO_WARNINGS

bool Log::CreateConsole() {
	if (!console_created && AllocConsole()) {
		freopen_s(&Log::fileout, "CONOUT$", "w", stdout);
		std::cout.clear();
		console_created = true;
		return console_created;
	}
	return true;
}

bool Log::InitLogging() {
#ifdef DEBUG
	// initialize the console
	if (!Log::CreateConsole()) {
		MessageBox(NULL, TEXT("Could not create Console"), TEXT("Error"), NULL);
	}
#endif // DEBUG
	std::string dir = Log::GetFileDirectory();
	dir += "ambria.log";
	//log_file.open(dir, std::ios::out | std::ios::app);
	log_file = fopen(dir.c_str(), "a");
	return true;
}

void Log::Cleanup() {
	if (console_created) {
		FreeConsole();
	}
	fclose(log_file);
	//log_file.close();
}

void Log::WriteLogHeader(std::stringstream &stream, Log::LOGLEVEL level) {
	std::time_t time = std::time(0);
	std::tm* now = std::localtime(&time);
	stream << (now->tm_year + 1900) << "/" << now->tm_mon << "/" << now->tm_mday << " " << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << " - ";
	switch (level) {
	case Log::LOGLEVEL::CRITICAL:
		stream << " [CRITICAL] ";
		break;
	case Log::LOGLEVEL::DEBUGG:
		stream << " [DEBUG] ";
		break;
	case Log::LOGLEVEL::INFO:
		stream << "[INFO] ";
		break;
	}
}

void Log::WriteLogHeader(std::wstringstream &stream, Log::LOGLEVEL level) {
	std::time_t time = std::time(0);
	std::tm* now = std::localtime(&time);
	stream << (now->tm_year + 1900) << L"/" << now->tm_mon << L"/" << now->tm_mday << L" " << now->tm_hour << L":" << now->tm_min << L":" << now->tm_sec << L" - ";
	switch (level) {
	case Log::LOGLEVEL::CRITICAL:
		stream << L" [CRITICAL] ";
		break;
	case Log::LOGLEVEL::DEBUGG:
		stream << L" [DEBUG] ";
		break;
	case Log::LOGLEVEL::INFO:
		stream << L"[INFO] ";
		break;
	}
}

/*
	log char
*/
void Log::LogMessage(char* message, int length, Log::LOGLEVEL level) {
	Log::LogMessageF("%.*s", level, length, message);
}

void Log::LogMessage(const char* message, Log::LOGLEVEL level) {
	Log::LogMessage((char*)message, strlen(message) * sizeof(char), level);
}

/*
	log wide char
*/
void Log::LogMessage(wchar_t* message, int length, Log::LOGLEVEL level) {
	Log::LogMessageF(L"%.*s", level, length, message);
}

void Log::LogMessage(const wchar_t* message, Log::LOGLEVEL level) {
	Log::LogMessage((wchar_t*)message, wcslen(message) * sizeof(wchar_t), level);
}

/*
	log string classes
*/

void Log::LogMessage(const std::string& message, Log::LOGLEVEL level) {
	Log::LogMessage(message.c_str(), level);
}

void Log::LogMessage(const std::wstring& message, Log::LOGLEVEL level) {
	Log::LogMessage(message.c_str(), level);
}

/*
	log with format
*/
void Log::LogMessageF(const char* format, Log::LOGLEVEL level, ...) {
	std::stringstream stream;
	Log::WriteLogHeader(stream, level);
	stream << format;

	va_list va;
	va_start(va, level);

	if(console_created)
		vprintf(stream.str().c_str(), va);
	vfprintf(log_file, stream.str().c_str(), va);
	va_end(va);
}

void Log::LogMessageF(const wchar_t* format, Log::LOGLEVEL level, ...) {
	std::wstringstream stream;
	Log::WriteLogHeader(stream, level);
	stream << format;

	va_list va;
	va_start(va, level);

	vwprintf(stream.str().c_str(), va);
	vfwprintf(log_file, stream.str().c_str(), va);
	va_end(va);
}

std::string Log::GetFileDirectory() {
	char path[MAX_PATH] = {NULL};
	GetModuleFileName(NULL, path, MAX_PATH);
	std::string* spath = NULL;
	for (int i = 0; i < MAX_PATH; i++) {
		int pos = MAX_PATH - i;
		if (path[pos] == '\\') {
			spath = new std::string(path, pos+1);
			break;
		}
	}
	return *spath;
}