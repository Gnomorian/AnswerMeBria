#include "AudioEndpointVolumeCallback.h"
#include "window.h"

#include <iostream>

AudioEndpointVolumeCallback::AudioEndpointVolumeCallback(void(*callback)()) : _cRef(1)
{
	_callback = callback;
}


AudioEndpointVolumeCallback::~AudioEndpointVolumeCallback()
{
}

// methods required by IUnknown
ULONG STDMETHODCALLTYPE AudioEndpointVolumeCallback::AddRef() {
	return InterlockedIncrement(&_cRef);
}

ULONG STDMETHODCALLTYPE AudioEndpointVolumeCallback::Release() {
	ULONG ulRef = InterlockedDecrement(&_cRef);
	if (0 == ulRef) {
		delete this;
	}
	return ulRef;
}

HRESULT STDMETHODCALLTYPE AudioEndpointVolumeCallback::QueryInterface(REFIID riid, VOID **ppvInterface) {
	if (IID_IUnknown == riid) {
		AddRef();
		*ppvInterface = (IUnknown*)this;
	}
	else if (__uuidof(IAudioEndpointVolumeCallback) == riid) {
		AddRef();
		*ppvInterface = (IAudioEndpointVolumeCallback*)this;
	}
	else {
		*ppvInterface = NULL;
		return E_NOINTERFACE;
	}
	return S_OK;
}

// callback method for endpoint-volume change notifications.
HRESULT STDMETHODCALLTYPE AudioEndpointVolumeCallback::OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify) {
	if (pNotify == NULL) {
		return E_INVALIDARG;
	}
	_callback();
}