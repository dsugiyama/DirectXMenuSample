#pragma once

#include <functional>
#include <Windows.h>
#include <d2d1_2.h>
#include <dwrite_2.h>

class Entry {
public:
	Entry(int left, int top, int right, int bottom,
		PCWSTR text, std::function<void ()> action);
	Entry(int left, int top, int right, int bottom,
		std::function<PCWSTR ()> textFunc, std::function<void ()> action);

	void Render(
		ID2D1HwndRenderTarget* pRenderTarget,
		ID2D1SolidColorBrush* pBrush,
		IDWriteTextFormat* pTextFormat);
	void DoAction();

private:
	D2D1_RECT_F rect;
	PCWSTR text;
	std::function<PCWSTR ()> textFunc;
	bool isTextFunc;
	std::function<void ()> action;
};

