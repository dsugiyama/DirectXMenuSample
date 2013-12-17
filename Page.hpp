#pragma once

#include <vector>
#include <Windows.h>
#include <d2d1_2.h>
#include <dwrite_2.h>
#include "Entry.hpp"
#include "SoundEffect.hpp"

class Page {
public:
	Page(PCWSTR title);

	void AddEntry(Entry entry);
	void SetSoundEffects(SoundEffect* pCursorMove,
		SoundEffect* pCursorEnter, SoundEffect* pCursorBack);
	bool SendKey(int keycode);
	void Render(
		ID2D1HwndRenderTarget* pRenderTarget,
		ID2D1SolidColorBrush* pNormalBrush,
		ID2D1SolidColorBrush* pFocusedBrush,
		IDWriteTextFormat* pNormalTextFormat,
		IDWriteTextFormat* pLargeTextFormat);

private:
	PCWSTR title;
	std::vector<Entry> entries;
	int focus = 0;

	SoundEffect* pSECursorMove;
	SoundEffect* pSECursorEnter;
	SoundEffect* pSECursorBack;
};

