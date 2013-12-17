#pragma once

#include <Windows.h>
#include <dsound.h>

class SoundEffect {
public:
	SoundEffect();

	HRESULT LoadFromResource(IDirectSound8* pDirectSound, int id, PCWSTR type);
	HRESULT Play();
	void Enable();
	void Disable();

private:
	IDirectSoundBuffer8* pBuffer;
	bool enabled;
};

