#include "AudioDeviceManager.h"


#include "AudioEndpointVolumeCallback.h"
#include <iostream>

AudioDeviceManager::AudioDeviceManager()
{
}


AudioDeviceManager::~AudioDeviceManager()
{
}

int AudioDeviceManager::InitializeCallback(AudioEndpointVolumeCallback* endpoint) {
	IMMDeviceEnumerator *devEnumerator = NULL;
	HRESULT hr = NULL;
	
	CoInitialize(NULL);

	// get the device enumerator
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, _uuidof(IMMDeviceEnumerator), (void**)&devEnumerator);
	if (hr != S_OK) {
		std::cout << "Could not get enumerator for endpoint devices" << std::endl;
	}

	// get all the output devices
	IMMDeviceCollection* devices;
	hr = devEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &devices);
	if (hr != S_OK) {
		std::cout << "Could not get array of divices" << std::endl;
	}

	// get the amount of devices
	UINT count;
	devices->GetCount(&count);

	for (int i = 0; i < count; i++) {
		LPWSTR deviceId = NULL;
		IMMDevice* device = NULL;
		IPropertyStore* props = NULL;

		devices->Item(i, &device);
		device->GetId(&deviceId);
		device->OpenPropertyStore(STGM_READ, &props);
		PROPVARIANT varName;
		PropVariantInit(&varName);
		// get the device name
		props->GetValue(PKEY_Device_FriendlyName, &varName);
		printf("Endpoint %d: \"%S\" (%S)\n", i, varName.pwszVal, deviceId);

		if (IsLogitechDevice(varName.pwszVal)) {
			std::cout << "Found Logitech Headset, Attaching" << std::endl;

			hr = device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&globalEndpoint);
			if (hr != S_OK) {
				std::cout << "Could not activate render device" << std::endl;
			}

			hr = globalEndpoint->RegisterControlChangeNotify((IAudioEndpointVolumeCallback*)endpoint);
			if (hr != S_OK) {
				std::cout << "Could not register callback" << std::endl;
			}
		}

		CoTaskMemFree(deviceId);
		deviceId = NULL;
		PropVariantClear(&varName);
		props->Release();
		device->Release();
	}
	return 0;
}

bool AudioDeviceManager::IsLogitechDevice(LPWSTR name) {
	std::wstring logitech(L"Logitech");
	std::wstring _name = name;
	if (_name.find(logitech) != std::string::npos) {
		return true;
	}
	return false;
}