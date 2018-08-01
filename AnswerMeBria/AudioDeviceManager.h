#pragma once

#include "AudioEndpointVolumeCallback.h"
#include <mmdeviceapi.h>
#include <Functiondiscoverykeys_devpkey.h>

class AudioDeviceManager
{
public:
	AudioDeviceManager();
	~AudioDeviceManager();
	int InitializeCallback(AudioEndpointVolumeCallback* endpoint);
private:
	bool IsLogitechDevice(LPWSTR name);

	GUID _myGuid = GUID_NULL;
	IAudioEndpointVolume *globalEndpoint;
	IMMDevice *_device;
};

