#include <cstring>
#include "BackgroundMusic.hpp"
#include "helper.hpp"

HRESULT BackgroundMusic::LoadFromResource(
	IDirectSound8* pDirectSound, int id, PCWSTR type)
{
	PVOID pMemory;
	ULONG size;
	auto hr = GetResource(id, type, &pMemory, &size);
	if (FAILED(hr)) { return hr; }

	this->pSource = (PBYTE)pMemory + 44;
	this->sourceSize = *(PUINT32)(this->pSource - 4);

	WAVEFORMATEX wfex = {};
	DSBUFFERDESC bufdesc = {};
	wfex.wFormatTag = WAVE_FORMAT_PCM;
	wfex.nSamplesPerSec = 44100;
	wfex.wBitsPerSample = 16;
	wfex.nChannels = 2;
	wfex.nBlockAlign = wfex.wBitsPerSample / 8 * wfex.nChannels;
	wfex.nAvgBytesPerSec = wfex.nSamplesPerSec * wfex.nBlockAlign;
	bufdesc.dwSize = sizeof(DSBUFFERDESC);
	bufdesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS |
		DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2;
	bufdesc.dwBufferBytes = BufferSizeCoef * wfex.nAvgBytesPerSec;
	bufdesc.lpwfxFormat = &wfex;

	IDirectSoundBuffer* pBuffer;
	hr = pDirectSound->CreateSoundBuffer(&bufdesc, &pBuffer, nullptr);
	if (SUCCEEDED(hr)) {
		hr = pBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&this->pBuffer);
		pBuffer->Release();
	}
	if (FAILED(hr)) { return hr; }

	this->bufferSize = bufdesc.dwBufferBytes;

	PVOID pWrite;
	DWORD length;
	hr = this->pBuffer->Lock(0, 0, &pWrite, &length, nullptr, nullptr, DSBLOCK_ENTIREBUFFER);
	if (FAILED(hr)) { return hr; }
	memcpy(pWrite, this->pSource, length);
	this->pBuffer->Unlock(pWrite, length, nullptr, 0);

	this->sourceOffset = length;

	hr = this->SetPeriodicEvents();
	return hr;
}

HRESULT BackgroundMusic::Play()
{
	auto hr = this->pBuffer->Play(0, 0, DSBPLAY_LOOPING);
	if (FAILED(hr)) { return hr; }

	this->timerId = timeSetEvent(TimerPeriod, TimerResolution,
		TimeProc, (DWORD_PTR)this, TIME_PERIODIC);
	return this->timerId == NULL ? E_FAIL : S_OK;
}

HRESULT BackgroundMusic::Pause()
{
	timeKillEvent(this->timerId);

	return this->pBuffer->Stop();
}

bool BackgroundMusic::IsPlaying()
{
	DWORD status;
	this->pBuffer->GetStatus(&status);
	return status & DSBSTATUS_PLAYING;
}

HRESULT BackgroundMusic::SetPeriodicEvents()
{
	IDirectSoundNotify8* pNotify;
	auto hr = this->pBuffer->QueryInterface(
		IID_IDirectSoundNotify8, (LPVOID*)&pNotify);
	if (FAILED(hr)) { return hr; }

	DSBPOSITIONNOTIFY positionNotifies[BufferSizeCoef];

	for (int i = 0; i < BufferSizeCoef; i++) {
		this->hPeriodicEvents[i] = CreateEventW(nullptr, FALSE, FALSE, nullptr);
		if (this->hPeriodicEvents[i] == nullptr) { return E_FAIL; }

		positionNotifies[i].dwOffset = bufferSize / BufferSizeCoef * (i + 1) - 1;
		positionNotifies[i].hEventNotify = this->hPeriodicEvents[i];
	}

	hr = pNotify->SetNotificationPositions(BufferSizeCoef, positionNotifies);
	this->currentEvent = 0;
	return hr;
}

HRESULT BackgroundMusic::WriteToBuffer()
{
	PVOID pWrite;
	DWORD length;
	int bytesPerSec = this->bufferSize / BufferSizeCoef;
	bool over = this->sourceOffset + bytesPerSec >= this->sourceSize;

	if (over) {
		auto hr = this->pBuffer->Lock(bytesPerSec * this->currentEvent,
			bytesPerSec, &pWrite, &length, nullptr, nullptr, 0);
		if (FAILED(hr)) { return hr; }
		int first_length = this->sourceSize - this->sourceOffset;
		memcpy(pWrite, this->pSource + this->sourceOffset, first_length);
		memcpy((PBYTE)pWrite + first_length, this->pSource, length - first_length);
		this->pBuffer->Unlock(pWrite, length, nullptr, 0);
		this->sourceOffset = length - first_length;
	} else {
		auto hr = this->pBuffer->Lock(bytesPerSec * this->currentEvent,
			bytesPerSec, &pWrite, &length, nullptr, nullptr, 0);
		if (FAILED(hr)) { return hr; }
		memcpy(pWrite, this->pSource + this->sourceOffset, length);
		this->pBuffer->Unlock(pWrite, length, nullptr, 0);
		this->sourceOffset += length;
	}

	return S_OK;
}

void CALLBACK BackgroundMusic::TimeProc(UINT uTimerID, UINT uMsg,
		DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
	auto pBgm = (BackgroundMusic*)dwUser;

	if (pBgm->timerId != uTimerID) {
		return;
	}

	WaitForSingleObject(pBgm->hPeriodicEvents[pBgm->currentEvent], INFINITE);

	pBgm->WriteToBuffer();

	pBgm->currentEvent = (pBgm->currentEvent + 1) % BufferSizeCoef;
}
