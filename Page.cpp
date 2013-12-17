#include "Page.hpp"

Page::Page(PCWSTR title) :
	title(title),
	pSECursorMove(nullptr),
	pSECursorEnter(nullptr),
	pSECursorBack(nullptr)
{
}

void Page::AddEntry(Entry entry)
{
	this->entries.push_back(entry);
}

void Page::SetSoundEffects(SoundEffect* pCursorMove,
	SoundEffect* pCursorEnter, SoundEffect* pCursorBack)
{
	this->pSECursorMove = pCursorMove;
	this->pSECursorEnter = pCursorEnter;
	this->pSECursorBack = pCursorBack;
}

bool Page::SendKey(int keycode)
{
	switch (keycode) {
	case VK_DOWN:
		this->focus = min(this->focus + 1,
			this->entries.size() - 1);
		if (this->pSECursorMove != nullptr) {
			this->pSECursorMove->Play();
		}
		return true;

	case VK_UP:
		this->focus = max(this->focus - 1, 0);
		if (this->pSECursorMove != nullptr) {
			this->pSECursorMove->Play();
		}
		return true;

	case VK_RETURN:
		this->entries[this->focus].DoAction();
		if (this->pSECursorEnter != nullptr) {
			this->pSECursorEnter->Play();
		}
		return true;

	default:
		return false;
	}
}

void Page::Render(
	ID2D1HwndRenderTarget* pRenderTarget,
	ID2D1SolidColorBrush* pNormalBrush,
	ID2D1SolidColorBrush* pFocusedBrush,
	IDWriteTextFormat* pNormalTextFormat,
	IDWriteTextFormat* pLargeTextFormat)
{
	auto size = pRenderTarget->GetSize();
	auto rect = D2D1::RectF(size.width / 2 - 250, 50,
		size.width / 2+ 250, 150);
	pRenderTarget->DrawTextW(this->title, lstrlenW(this->title),
		pLargeTextFormat, rect, pNormalBrush);

	for (int i = 0; i < this->entries.size(); i++) {
		auto pBrush = i == this->focus ?
			pFocusedBrush : pNormalBrush;
		entries[i].Render(pRenderTarget, pBrush, pNormalTextFormat);
	}
}
