#include <cstring>
#include "SoundEffect.hpp"
#include "helper.hpp"

SoundEffect::SoundEffect() :
	enabled(true)
{
}

HRESULT SoundEffect::LoadFromResource(
	IDirectSound8* pDirectSound, int id, PCWSTR type)
{
	PVOID pMemory;
	ULONG size;
	auto hr = GetResource(id, type, &pMemory, &size);
	if (FAILED(hr)) { return hr; }

	auto dataSize = *(PUINT32)((PBYTE)pMemory + 40);

	WAVEFORMATEX wfex = {};
	DSBUFFERDESC bufdesc = {};
	wfex.wFormatTag = WAVE_FORMAT_PCM;
	wfex.nSamplesPerSec = 44100;
	wfex.wBitsPerSample = 16;
	wfex.nChannels = 2;
	wfex.nBlockAlign = wfex.wBitsPerSample / 8 * wfex.nChannels;
	wfex.nAvgBytesPerSec = wfex.nSamplesPerSec * wfex.nBlockAlign;
	bufdesc.dwSize = sizeof(DSBUFFERDESC);
	bufdesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS;
	bufdesc.dwBufferBytes = dataSize;
	bufdesc.lpwfxFormat = &wfex;

	IDirectSoundBuffer* pBuffer;
	hr = pDirectSound->CreateSoundBuffer(&bufdesc, &pBuffer, nullptr);
	if (SUCCEEDED(hr)) {
		hr = pBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&this->pBuffer);
		pBuffer->Release();
	}
	if (FAILED(hr)) { return hr; }

	PVOID pWrite;
	DWORD length;

	hr = this->pBuffer->Lock(0, 0, &pWrite, &length, nullptr, nullptr, DSBLOCK_ENTIREBUFFER);
	if (FAILED(hr)) { return hr; }
	auto pData = reinterpret_cast<PVOID>(reinterpret_cast<ULONG64>(pMemory) + 44);
	memcpy(pWrite, pData, length);
	this->pBuffer->Unlock(pWrite, length, nullptr, 0);

	return S_OK;
}

HRESULT SoundEffect::Play()
{
	if (this->enabled) {
		this->pBuffer->Stop();
		this->pBuffer->SetCurrentPosition(0);
		auto hr = this->pBuffer->Play(0, 0, 0);
		return hr;
	}
}

void SoundEffect::Enable()
{
	this->enabled = true;
}

void SoundEffect::Disable()
{
	this->enabled = false;
}
