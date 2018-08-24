#pragma once

#include <winreg.h>

class Registry
{
public:
	Registry();
	~Registry();
	void GetSettings();
	void ChangeSetting(LPCSTR valueName, const char* value);
	TCHAR* GetExecutablePath();
	void RegisterStartup(bool launchOnStartup = true);

	bool _is_running_at_startup = false;
private:
	TCHAR* _startup = NULL;
	HKEY _application_key = NULL;

	LPCSTR startup_subkey = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	LPCSTR startup_value_name = "answermebria";
};

