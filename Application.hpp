#pragma once

#include <vector>
#include <Windows.h>
#include <d2d1_2.h>
#include <dwrite_2.h>
#include <dsound.h>
#include <wincodec.h>
#include <wincodecsdk.h>
#include "Page.hpp"
#include "BackgroundMusic.hpp"
#include "SoundEffect.hpp"

class Application {
public:
	Application();
	~Application();

	HRESULT Initialize(HWND hWnd);
	bool SendKey(int keycode);
	HRESULT Render();

private:
	const D2D1_COLOR_F NormalColor =
		D2D1::ColorF(D2D1::ColorF::LightGray);
	const D2D1_COLOR_F FocusedColor =
		D2D1::ColorF(D2D1::ColorF::Yellow);
	PCWSTR FontFamily = L"Segoe UI";
	const float NormalFontSize = 20.0f;
	const float LargeFontSize = 46.0f;

	ID2D1Factory2* pD2DFactory;
	IDWriteFactory2* pDWriteFactory;
	ID2D1HwndRenderTarget* pRenderTarget;
	ID2D1SolidColorBrush* pNormalBrush;
	ID2D1SolidColorBrush* pFocusedBrush;
	IDWriteTextFormat* pNormalTextFormat;
	IDWriteTextFormat* pLargeTextFormat;
	IWICImagingFactory* pImagingFactory;
	ID2D1Bitmap* pBackgroundBitmap;
	IDirectSound8* pDirectSound;

	HWND hHostWnd;
	Page startPage;
	Page pageA, pageB, pageC;
	Page pageA1, pageA2;
	Page* pCurrentPage;

	BackgroundMusic bgm;
	SoundEffect seCursorMove;
	SoundEffect seCursorEnter;
	SoundEffect seCursorBack;
	bool seOn;

	void InitializeUIComponents();
	HRESULT InitializeSounds();
	HRESULT CreateDeviceResources();
	void ReleaseDeviceResources();
	void RenderBackground();
};

