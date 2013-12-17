#include <Windows.h>
#include "AppWindow.hpp"

int WINAPI wWinMain(
	HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PWSTR pCmdLine, int nCmdShow)
{
	auto hr = CoInitialize(nullptr);
	if (FAILED(hr)) {
		return -1;
	}

	AppWindow window;
	hr = window.Create(hInstance);
	if (FAILED(hr)) {
		return -1;
	}

	window.Show(nCmdShow);
	window.RunMessageLoop();

	CoUninitialize();

	return 0;
}
