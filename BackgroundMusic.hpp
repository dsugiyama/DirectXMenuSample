#pragma once

#include <Windows.h>
#include <dsound.h>

class BackgroundMusic {
public:
	HRESULT LoadFromResource(IDirectSound8* pDirectSound, int id, PCWSTR type);
	HRESULT Play();
	HRESULT Pause();
	bool IsPlaying();

private:
	static const int BufferSizeCoef = 3;
	static const int TimerPeriod = 500;
	static const int TimerResolution = 100;

	IDirectSoundBuffer8* pBuffer;
	PBYTE pSource;
	UINT32 bufferSize;
	UINT32 sourceSize;
	UINT32 sourceOffset;
	HANDLE hPeriodicEvents[BufferSizeCoef];
	MMRESULT timerId;
	int currentEvent;

	HRESULT SetPeriodicEvents();
	HRESULT WriteToBuffer();

	static void CALLBACK TimeProc(UINT uID, UINT uMsg,
		DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);
};

