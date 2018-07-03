#include "Logger.h"

// alocates a console to this windows application
bool Log::CreateConsole() {
	if (!created && AllocConsole()) {
		freopen_s(&Log::fileout, "CONOUT$", "w", stdout);
		std::cout.clear();
		created = true;
		return created;
	}
	return true;
}

void Log::Cleanup() {
	if (created) {
		FreeConsole();
	}
}

// writes to the console, returns true if successful
// TODO: add time and date to the output
bool Log::LogMessage(const std::string &message, Log::LOGLEVEL level) {
	if (created) {
		switch (level) {
		case Log::LOGLEVEL::INFO:
			std::cout << " [INFO] ";
			break;
		case Log::LOGLEVEL::IMPORTANT:
			std::cout << " [IMPORTANT] ";
			break;
		case Log::LOGLEVEL::CRITICAL:
			std::cout << " [CRITICAL] ";
			break;
		}
		std::cout << message << std::endl;
	}
	return false;
}