#pragma once

#include <Windows.h>
#include <d2d1_2.h>
#include <wincodec.h>
#include <wincodecsdk.h>

class Bitmap {
public:
	static HRESULT LoadFromResource(
		ID2D1RenderTarget* pRenderTarget,
		IWICImagingFactory* pImagingFactory,
		ID2D1Bitmap** ppBitmap,
		int id, PWSTR type);
};

