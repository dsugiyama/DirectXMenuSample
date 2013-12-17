#include <d2d1_2helper.h>
#include "resource.h"
#include "Application.hpp"
#include "Bitmap.hpp"
#include "helper.hpp"

Application::Application() :
	pD2DFactory(nullptr),
	pDWriteFactory(nullptr),
	pRenderTarget(nullptr),
	pNormalBrush(nullptr),
	pFocusedBrush(nullptr),
	pNormalTextFormat(nullptr),
	pLargeTextFormat(nullptr),
	pImagingFactory(nullptr),
	pBackgroundBitmap(nullptr),
	pDirectSound(nullptr),
	startPage(L"Start Page"),
	pageA(L"Page A"),
	pageB(L"Page B"),
	pageC(L"Sound Settings"),
	pageA1(L"Page A-1"),
	pageA2(L"Page A-2"),
	seOn(true)
{
}

Application::~Application()
{
	SafeRelease(&this->pD2DFactory);
	SafeRelease(&this->pDWriteFactory);
	SafeRelease(&this->pRenderTarget);
	SafeRelease(&this->pNormalBrush);
	SafeRelease(&this->pFocusedBrush);
	SafeRelease(&this->pNormalTextFormat);
	SafeRelease(&this->pLargeTextFormat);
	SafeRelease(&this->pImagingFactory);
	SafeRelease(&this->pBackgroundBitmap);
	SafeRelease(&this->pDirectSound);
}

HRESULT Application::Initialize(HWND hWnd)
{
	this->hHostWnd = hWnd;

	auto hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		&this->pD2DFactory);
	if (FAILED(hr)) { return hr; }

	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory2),
		reinterpret_cast<IUnknown**>(&this->pDWriteFactory));
	if (FAILED(hr)) { return hr; }

	hr = this->pDWriteFactory->CreateTextFormat(
		FontFamily, nullptr,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		NormalFontSize, L"en-us",
		&this->pNormalTextFormat);
	if (FAILED(hr)) { return hr; }
    hr = this->pNormalTextFormat->SetTextAlignment(
		DWRITE_TEXT_ALIGNMENT_CENTER);
	if (FAILED(hr)) { return hr; }
    hr = this->pNormalTextFormat->SetParagraphAlignment(
		DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	if (FAILED(hr)) { return hr; }

	hr = this->pDWriteFactory->CreateTextFormat(
		FontFamily, nullptr,
		DWRITE_FONT_WEIGHT_SEMI_LIGHT,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		LargeFontSize, L"en-us",
		&this->pLargeTextFormat);
	if (FAILED(hr)) { return hr; }
    hr = this->pLargeTextFormat->SetTextAlignment(
		DWRITE_TEXT_ALIGNMENT_CENTER);
	if (FAILED(hr)) { return hr; }
    hr = this->pLargeTextFormat->SetParagraphAlignment(
		DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	if (FAILED(hr)) { return hr; }

	hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		reinterpret_cast<void**>(&pImagingFactory));
	if (FAILED(hr)) { return hr; }

	hr = DirectSoundCreate8(nullptr, &this->pDirectSound, nullptr);
	if (SUCCEEDED(hr)) {
		hr = this->pDirectSound->SetCooperativeLevel(
			this->hHostWnd, DSSCL_PRIORITY);
	}
	if (FAILED(hr)) { return hr; }

	hr = this->InitializeSounds();
	if (FAILED(hr)) { return hr; }

	this->InitializeUIComponents();

	return S_OK;

}

bool Application::SendKey(int keycode)
{
	return this->pCurrentPage->SendKey(keycode);
}

HRESULT Application::Render()
{
	auto hr = this->CreateDeviceResources();

	if (SUCCEEDED(hr)) {
		this->pRenderTarget->BeginDraw();

		this->pRenderTarget->Clear(
			D2D1::ColorF(D2D1::ColorF::Black));

		this->RenderBackground();

		this->pCurrentPage->Render(
			this->pRenderTarget,
			this->pNormalBrush,
			this->pFocusedBrush,
			this->pNormalTextFormat,
			this->pLargeTextFormat
			);

		hr = this->pRenderTarget->EndDraw();
	}

	if (hr == D2DERR_RECREATE_TARGET) {
		this->ReleaseDeviceResources();
		hr = S_OK;
	}

	return hr;
}

HRESULT Application::InitializeSounds()
{
	auto hr = this->bgm.LoadFromResource(
		this->pDirectSound, IDR_WAV1, L"WAV");
	if (FAILED(hr)) { return hr; }
	this->bgm.Play();

	hr = this->seCursorMove.LoadFromResource(
		this->pDirectSound, IDR_WAV2, L"WAV");
	if (FAILED(hr)) { return hr; }

	this->seCursorEnter.LoadFromResource(
		this->pDirectSound, IDR_WAV3, L"WAV");
	if (FAILED(hr)) { return hr; }

	this->seCursorBack.LoadFromResource(
		this->pDirectSound, IDR_WAV4, L"WAV");
	return hr;
}

void Application::InitializeUIComponents()
{
	RECT rect;
	GetClientRect(this->hHostWnd, &rect);
	auto size = D2D1::SizeU(rect.right - rect.left, rect.bottom - rect.top);

	Entry toPageA(size.width / 2 - 200, 200, size.width / 2 + 200, 250,
		L"Go to page A", [this]() {
			this->pCurrentPage = &this->pageA;
		});
	Entry toPageB(size.width / 2 - 200, 300, size.width / 2 + 200, 350,
		L"Go to page B", [this]() {
			this->pCurrentPage = &this->pageB;
		});
	Entry toPageC(size.width / 2 - 200, 400, size.width / 2 + 200, 450,
		L"Sound Settings", [this]() {
			this->pCurrentPage = &this->pageC;
		});
	Entry toPageA1(size.width / 2 - 200, 200, size.width / 2 + 200, 250,
		L"Go to page A-1", [this]() {
			this->pCurrentPage = &this->pageA1;
		});
	Entry toPageA2(size.width / 2 - 200, 300, size.width / 2 + 200, 350,
		L"Go to page A-2", [this]() {
			this->pCurrentPage = &this->pageA2;
		});
	Entry backToStartPage(50, 500, 150, 550, L"Back",
		[this]() {
			this->pCurrentPage = &this->startPage;
		});
	Entry backToPageA(50, 500, 150, 550, L"Back",
		[this]() {
			this->pCurrentPage = &this->pageA;
		});
	Entry toggleBgm(size.width / 2 - 200, 200, size.width / 2 + 200, 250,
		[this]() {
			return this->bgm.IsPlaying() ? L"BGM: ON" : L"BGM: OFF";
		}, [this]() {
			if (this->bgm.IsPlaying()) {
				this->bgm.Pause();
			} else {
				this->bgm.Play();
			}
		});
	Entry toggleSE(size.width / 2 - 200, 300, size.width / 2 + 200, 350,
		[this]() {
			return this->seOn ? L"SE: ON" : L"SE: OFF";
		}, [this]() {
			if (this->seOn) {
				this->seOn = false;
				this->seCursorMove.Disable();
				this->seCursorEnter.Disable();
				this->seCursorBack.Disable();
			} else {
				this->seOn = true;
				this->seCursorMove.Enable();
				this->seCursorEnter.Enable();
				this->seCursorBack.Enable();
			}
		});

	this->startPage.AddEntry(toPageA);
	this->startPage.AddEntry(toPageB);
	this->startPage.AddEntry(toPageC);
	this->pageA.AddEntry(toPageA1);
	this->pageA.AddEntry(toPageA2);
	this->pageA.AddEntry(backToStartPage);
	this->pageB.AddEntry(backToStartPage);
	this->pageC.AddEntry(toggleBgm);
	this->pageC.AddEntry(toggleSE);
	this->pageC.AddEntry(backToStartPage);
	this->pageA1.AddEntry(backToPageA);
	this->pageA2.AddEntry(backToPageA);

	this->pCurrentPage = &this->startPage;

	this->startPage.SetSoundEffects(&this->seCursorMove,
		&this->seCursorEnter, nullptr);
	this->pageA.SetSoundEffects(&this->seCursorMove,
		&this->seCursorEnter, nullptr);
	this->pageB.SetSoundEffects(&this->seCursorMove,
		&this->seCursorEnter, nullptr);
	this->pageC.SetSoundEffects(&this->seCursorMove,
		&this->seCursorEnter, nullptr);
	this->pageA1.SetSoundEffects(&this->seCursorMove,
		&this->seCursorEnter, nullptr);
	this->pageA2.SetSoundEffects(&this->seCursorMove,
		&this->seCursorEnter, nullptr);
}

HRESULT Application::CreateDeviceResources()
{
	if (this->pRenderTarget != nullptr) {
		return S_OK;
	}

	RECT rect;
	GetClientRect(this->hHostWnd, &rect);
	auto size = D2D1::SizeU(rect.right - rect.left, rect.bottom - rect.top);

	auto hr = this->pD2DFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(this->hHostWnd, size),
		&this->pRenderTarget);
	if (FAILED(hr)) { return hr; }

	hr = this->pRenderTarget->CreateSolidColorBrush(
		NormalColor, &this->pNormalBrush);
	if (FAILED(hr)) { return hr; }

	hr = this->pRenderTarget->CreateSolidColorBrush(
		FocusedColor, &this->pFocusedBrush);
	if (FAILED(hr)) { return hr; }

	hr = Bitmap::LoadFromResource(
		this->pRenderTarget,
		this->pImagingFactory,
		&this->pBackgroundBitmap,
		IDB_PNG1, L"PNG");
	return hr;
}

void Application::ReleaseDeviceResources()
{
	SafeRelease(&this->pRenderTarget);
	SafeRelease(&this->pNormalBrush);
	SafeRelease(&this->pFocusedBrush);
}

void Application::RenderBackground()
{
	auto size = this->pRenderTarget->GetSize();
	auto rect = D2D1::Rect<float>(0, 0, size.width, size.height);
	this->pRenderTarget->DrawBitmap(this->pBackgroundBitmap, rect);
}
