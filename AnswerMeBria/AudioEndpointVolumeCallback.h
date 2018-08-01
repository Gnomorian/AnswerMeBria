#pragma once

#include <endpointvolume.h>

//-----------------------------------------------------------
// Client implementation of IAudioEndpointVolumeCallback
// interface. When a method in the IAudioEndpointVolume
// interface changes the volume level or muting state of the
// endpoint device, the change initiates a call to the
// client's IAudioEndpointVolumeCallback::OnNotify method.
//-----------------------------------------------------------
class AudioEndpointVolumeCallback : public IAudioEndpointVolumeCallback
{
	LONG _cRef;
public:
	AudioEndpointVolumeCallback(void(*callback)());
	~AudioEndpointVolumeCallback();
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, VOID **ppvInterface);
	HRESULT STDMETHODCALLTYPE OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify);
private:
	void(*_callback)();
};

