#pragma once

#include "common.h"
#include <fstream>
#include <sstream>

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

	// writes to the stream the format for the current time and date
	void WriteLogHeader(std::stringstream &stream, Log::LOGLEVEL level);
	void WriteLogHeader(std::wstringstream &stream, Log::LOGLEVEL level);

	// write non null terminated messages to the console and logfile, returns true of successful
	bool LogChunk(char *buffer, size_t startpos, size_t length, Log::LOGLEVEL level, bool endline = true);

	/*
		log char
	*/
	void LogMessage(char* message, int length, Log::LOGLEVEL level = Log::LOGLEVEL::INFO);

	void LogMessage(const char* message, Log::LOGLEVEL level = Log::LOGLEVEL::INFO);

	/*
		log wide char
	*/
	void LogMessage(wchar_t* message, int length, Log::LOGLEVEL level = Log::LOGLEVEL::INFO);

	void LogMessage(const wchar_t* message, Log::LOGLEVEL level = Log::LOGLEVEL::INFO);

	/*
		log with format
	*/
	void LogMessageF(const char* format, Log::LOGLEVEL level = Log::LOGLEVEL::INFO, ...);

	void LogMessageF(const wchar_t* format, Log::LOGLEVEL level = Log::LOGLEVEL::INFO, ...);
}