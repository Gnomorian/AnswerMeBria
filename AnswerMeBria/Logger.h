#pragma once

#include "common.h"
#include <fstream>

namespace Log {

	enum LOGLEVEL {
		INFO,		// logged to file, app start, end, bria call answered, missed call etc.
		DEBUGG,		// only apears in log file and console when using debug binaries/
		CRITICAL	// log messages that cause the application to fail.
	};

	static bool console_created = false;
	// virual file used for the console
	static FILE* fileout = NULL;
	static std::fstream log_file;

	// alocates a console to this windows application
	// used when debugging to see more verbose messages
	bool CreateConsole();

	// open log files and console where required
	bool InitLogging();

	// deallocates the console and closes files
	void Cleanup();

	// writes to the console and logfile, returns true if successful
	bool LogMessage(const std::string &message, Log::LOGLEVEL level = Log::LOGLEVEL::INFO);
}