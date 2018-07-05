#include "Logger.h"
#define _CRT_SECURE_NO_WARNINGS
#include <ctime>

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
	LogMessage("Launched Ambria...");
	return true;
}

void Log::Cleanup() {
	if (console_created) {
		FreeConsole();
	}
}

bool Log::LogMessage(const std::string &message, Log::LOGLEVEL level) {

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
		std::cout << message << std::endl;
	}
	return false;
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