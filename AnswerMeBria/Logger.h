#pragma once

#include "common.h"

namespace Log {

	enum LOGLEVEL {
		INFO,
		IMPORTANT,
		CRITICAL
	};

	static bool created = false;
	static FILE* fileout = NULL;

	bool CreateConsole();
	void Cleanup();
	bool LogMessage(const std::string &message, Log::LOGLEVEL level = Log::LOGLEVEL::INFO);
}