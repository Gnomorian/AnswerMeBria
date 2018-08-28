#include "common.h"
#include "Registry.h"


// sets up handles and default values
Registry::Registry()
{
	LSTATUS stat = RegOpenKeyExA(HKEY_CURRENT_USER, startup_subkey, 0, KEY_ALL_ACCESS, &_application_key);
	if (stat != ERROR_SUCCESS) {
		Log::LogMessageF("Could not get registry application_key. (%i)", Log::LOGLEVEL::CRITICAL, stat);
		return;
	}

	_startup = GetExecutablePath();
	GetSettings();
}

// cleanup handles
Registry::~Registry() {
	// close the key after use
	if (_application_key != NULL)
		RegCloseKey(_application_key);
	if (_startup)
		delete[] _startup;
}

// gets the values that already exist in the registry
void Registry::GetSettings() {
	LSTATUS rtrn = 0;
	byte data[MAX_PATH] = {NULL};
	DWORD buffersize = MAX_PATH;
	DWORD valueType = NULL;

	rtrn = RegQueryValueEx(_application_key, startup_value_name, NULL, &valueType, data, &buffersize);

	switch (rtrn) {
	case 2: // the value didnt exist
		Log::LogMessage("No Registry key, We are not running at startup.\n");
		break;
	case 0: // the value existed, set my value to what was in the registry
		_is_running_at_startup = true;
		memset(_startup, NULL, MAX_PATH);
		strcpy(_startup, (TCHAR*)data);
		break;
	default: // some other error code that would be an issue
		Log::LogMessageF("Could not query the application statup registry key.(%i)", Log::LOGLEVEL::CRITICAL, rtrn);
		return;
	}
}

// change the value that exists in the registry
void Registry::ChangeSetting(LPCSTR valueName, const char* value) {
	LSTATUS rtrn = 0;
	rtrn = RegSetValueEx(_application_key, valueName, 0, REG_SZ, (const byte*)value, strlen(value));
	if (rtrn != 0) {
		Log::LogMessageF("Could not set the value of %s (%i)", Log::LOGLEVEL::CRITICAL, valueName, rtrn);
	}
}

void Registry::RegisterStartup(bool launchOnStartup) {
	if(launchOnStartup) {
		Log::LogMessageF("Setting launch at startup to true. (%s:%s)", Log::LOGLEVEL::INFO, startup_subkey, _startup);

		ChangeSetting(startup_value_name, _startup);
	}
	else {
		LSTATUS status = RegDeleteKeyValueA(HKEY_CURRENT_USER, startup_subkey, startup_value_name);
		Log::LogMessageF("Deleting reigstry key so it doesnt launch at startup (%i)", Log::LOGLEVEL::INFO, status);
	}
	_is_running_at_startup = launchOnStartup;
}

TCHAR* Registry::GetExecutablePath() {
	TCHAR* name = new TCHAR[MAX_PATH];
	DWORD rtrn = GetModuleFileName(NULL, name, MAX_PATH);
	DWORD err = GetLastError();
	if (err != 0) {
		Log::LogMessageF("Could not get exe name. (%i)", Log::LOGLEVEL::CRITICAL, err);
		return NULL;
	}
	return name;
}