#pragma once

#include <Windows.h>

template <class Interface>
inline void SafeRelease(Interface **ppInterface)
{
	if (*ppInterface != NULL) {
		(*ppInterface)->Release();
		(*ppInterface) = nullptr;
	}
}

HRESULT GetResource(int id, PCWSTR type, PVOID* ppMemory, ULONG* pSize);
