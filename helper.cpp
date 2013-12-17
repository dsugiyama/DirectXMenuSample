#include "helper.hpp"

HRESULT GetResource(int id, PCWSTR type, PVOID* ppMemory, ULONG* pSize)
{
	if (ppMemory == nullptr || pSize == nullptr) { return E_FAIL; }

	auto hResInfo = FindResourceW(nullptr, MAKEINTRESOURCEW(id), type);
	if (hResInfo == nullptr) { return E_FAIL; }

	auto hResData = LoadResource(nullptr, hResInfo);
	if (hResData == nullptr) { return E_FAIL; }

	*ppMemory = LockResource(hResData);
	if (*ppMemory == nullptr) { return E_FAIL; }

	*pSize = SizeofResource(nullptr, hResInfo);
	return *pSize == 0 ? E_FAIL : S_OK;
}
