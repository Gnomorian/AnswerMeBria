#include "Logger.h"
#define _CRT_SECURE_NO_WARNINGS
#include <ctime>

#define LogMessagewf(s, ...) wprintf(s, __VA_ARGS__)
#define LogMessagef(s, ...) printf(s, __VA_ARGS__)


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
	log_file.open("ambria.log", std::ios::ate);
	return true;
}

void Log::Cleanup() {
	if (console_created) {
		FreeConsole();
	}
}

void Log::WriteLogHeader(std::stringstream &stream, Log::LOGLEVEL level) {
	std::time_t time = std::time(0);
	std::tm* now = std::localtime(&time);
	stream << now->tm_year << "/" << now->tm_mday << "/" << now->tm_wday << " " << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << " - ";
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
	stream << now->tm_year << L"/" << now->tm_mday << L"/" << now->tm_wday << L" " << now->tm_hour << L":" << now->tm_min << L":" << now->tm_sec << L" - ";
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
	std::stringstream stream;
	Log::WriteLogHeader(stream, level);
	stream << message, length;
	//stream.write(message, length);
	if(console_created)
		std::cout << stream.str() << std::endl;
	log_file << stream.str() << std::endl;
}

void Log::LogMessage(const char* message, Log::LOGLEVEL level) {
	Log::LogMessage((char*)message, strlen(message) * sizeof(char), level);
}

/*
	log wide char
*/
void Log::LogMessage(wchar_t* message, int length, Log::LOGLEVEL level) {
	std::wstringstream stream;
	Log::WriteLogHeader(stream, level);
	stream << message, length;
	//stream.write(message, length);
	if (console_created)
		std::wcout << stream.str() << std::endl;
	log_file << stream.str().c_str() << std::endl;
}

void Log::LogMessage(const wchar_t* message, Log::LOGLEVEL level) {
	Log::LogMessage((wchar_t*)message, wcslen(message) * sizeof(wchar_t), level);
}

/*
	log with format
*/
void Log::LogMessageF(const char* format, Log::LOGLEVEL level, ...) {
	std::stringstream stream;
	Log::WriteLogHeader(stream, level);

}

void Log::LogMessageF(const wchar_t* format, Log::LOGLEVEL level, ...) {

}

bool Log::LogChunk(char *buffer, size_t startpos, size_t length, Log::LOGLEVEL level, bool endline) {
	// add the date and time prefix
	std::time_t time = std::time(0);
	std::tm* now = std::localtime(&time);
	printf("%i/%i/%i %i:%i:%i -", now->tm_year, now->tm_mon, now->tm_wday, now->tm_hour, now->tm_min, now->tm_sec);

	if (console_created) {
		switch (level) {
		case Log::LOGLEVEL::INFO:
			std::cout << " [INFO] ";
			break;
		case Log::LOGLEVEL::DEBUGG:
#ifdef DEBUG
			std::cout << " [DEBUG] ";
#endif // DEBUG
			break;
		case Log::LOGLEVEL::CRITICAL:
			std::cout << " [CRITICAL] ";
			break;
		}
		std::cout.write(buffer + startpos, length);
		if(endline)
			std::cout << std::endl;
	}
	return false;
}