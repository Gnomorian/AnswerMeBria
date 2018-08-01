#pragma once

#include "Pipe.h"

class BriaAPI
{
public:
	BriaAPI();
	~BriaAPI();
	// ask bria for current call status information
	int GetCallStatus(std::string &responce, Pipe& pipe);
	int AnswerCall(Pipe& pipe, std::string id, std::string& responce);
	bool HasRingingCall(std::string& responce);
	int GetCallerId(std::string &responce, std::string &callerId);
};

