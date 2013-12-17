#include <atlbase.h>
#include "Bitmap.hpp"

HRESULT Bitmap::LoadFromResource(
	ID2D1RenderTarget* pRenderTarget,
	IWICImagingFactory* pImagingFactory,
	ID2D1Bitmap** ppBitmap,
	int id, PWSTR type)
{
	auto hResInfo = FindResourceW(nullptr, MAKEINTRESOURCEW(id), type);
	if (hResInfo == nullptr) { return E_FAIL; }

	auto hResData = LoadResource(nullptr, hResInfo);
	if (hResData == nullptr) { return E_FAIL; }

	auto pMemory = LockResource(hResData);
	if (pMemory == nullptr) { return E_FAIL; }

	auto size = SizeofResource(nullptr, hResInfo);
	if (size == 0) { return E_FAIL; }

	CComPtr<IWICStream> pStream;
	auto hr = pImagingFactory->CreateStream(&pStream);
	if (FAILED(hr)) { return hr; }
	hr = pStream->InitializeFromMemory((PBYTE)pMemory, size);
	if (FAILED(hr)) { return hr; }

	CComPtr<IWICBitmapDecoder> pDecoder;
	hr = pImagingFactory->CreateDecoderFromStream(pStream, nullptr,
		WICDecodeMetadataCacheOnDemand, &pDecoder);
	if (FAILED(hr)) { return hr; }

	CComPtr<IWICBitmapFrameDecode> pFrame;
	hr = pDecoder->GetFrame(0, &pFrame);
	if (FAILED(hr)) { return hr; }

	CComPtr<IWICFormatConverter> pConverter;
	hr = pImagingFactory->CreateFormatConverter(&pConverter);
	if (FAILED(hr)) { return hr; }
	hr = pConverter->Initialize(pFrame, GUID_WICPixelFormat32bppPRGBA,
		WICBitmapDitherTypeNone, nullptr, 0,
		WICBitmapPaletteTypeMedianCut);
	if (FAILED(hr)) { return hr; }

	hr = pRenderTarget->CreateBitmapFromWicBitmap(pConverter, nullptr, ppBitmap);
	return hr;
}
