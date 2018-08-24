#include "common.h"
#include "Registry.h"


// sets up handles and default values
Registry::Registry()
{
	LSTATUS stat = RegOpenKeyExA(HKEY_CURRENT_USER, startup_subkey, 0, KEY_ALL_ACCESS, &_application_key);
	if (stat != ERROR_SUCCESS) {
		printf("Could not get registry application_key. (%i)\n", stat);
		return;
	}

	_startup = GetExecutablePath();
	GetSettings();
	if (_startup != NULL) {
		printf(_startup);
	}
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
		printf("No Registry key, We are not running at startup.\n");
		break;
	case 0: // the value existed, set my value to what was in the registry
		_is_running_at_startup = true;
		memset(_startup, NULL, MAX_PATH);
		strcpy(_startup, (TCHAR*)data);
		break;
	default: // some other error code that would be an issue
		printf("Could not query the application statup registry key. %i\n", rtrn);
		return;
	}
}

// change the value that exists in the registry
void Registry::ChangeSetting(LPCSTR valueName, const char* value) {
	LSTATUS rtrn = 0;
	rtrn = RegSetValueEx(_application_key, valueName, 0, REG_SZ, (const byte*)value, strlen(value));
	if (rtrn != 0) {
		printf("Could not Set the value of %s. %i", valueName, rtrn);
	}
}

void Registry::RegisterStartup(bool launchOnStartup) {
	if(launchOnStartup) {
		printf("Setting launch at startup to true: %s %s\n", startup_subkey, _startup);
		ChangeSetting(startup_value_name, _startup);
	}
	else {
		LSTATUS status = RegDeleteKeyValueA(HKEY_CURRENT_USER, startup_subkey, startup_value_name);
		printf("Deleting reigstry key so it doesnt launch at startup (%i)\n", status);
	}
	_is_running_at_startup = launchOnStartup;
}

TCHAR* Registry::GetExecutablePath() {
	TCHAR* name = new TCHAR[MAX_PATH];
	DWORD rtrn = GetModuleFileName(NULL, name, MAX_PATH);
	DWORD err = GetLastError();
	if (err != 0) {
		printf("Could not get exe name. %d\n", err);
		return NULL;
	}
	return name;
}