#include "Entry.hpp"

Entry::Entry(int left, int top, int right, int bottom,
	PCWSTR text, std::function<void ()> action) :
	rect(D2D1::RectF(left, top, right, bottom)),
	action(action),
	text(text),
	isTextFunc(false)
{
}

Entry::Entry(int left, int top, int right, int bottom,
	std::function<PCWSTR ()> textFunc, std::function<void ()> action) :
	rect(D2D1::RectF(left, top, right, bottom)),
	action(action),
	textFunc(textFunc),
	isTextFunc(true)
{
}

void Entry::Render(
	ID2D1HwndRenderTarget* pRenderTarget,
	ID2D1SolidColorBrush* pBrush,
	IDWriteTextFormat* pTextFormat)
{
	pRenderTarget->DrawRectangle(this->rect, pBrush);
	auto text = this->isTextFunc ? this->textFunc() : this->text;
	pRenderTarget->DrawTextW(text, lstrlenW(text),
		pTextFormat, this->rect, pBrush);
}

void Entry::DoAction()
{
	this->action();
}
