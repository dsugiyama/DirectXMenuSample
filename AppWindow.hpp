#pragma once

#include <Windows.h>
#include "Application.hpp"

class AppWindow {
public:
	PCWSTR ClassName = L"AppWindow";
	PCWSTR Title = L"DirectX Menu Sample";
	const int Width = 800;
	const int Height = 600;

	HRESULT Create(HINSTANCE hInstance);
	void Show(int nCmdShow);
	void RunMessageLoop();

private:
	HWND hWnd;
	Application app;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg,
		WPARAM wParam, LPARAM lParam);
};

