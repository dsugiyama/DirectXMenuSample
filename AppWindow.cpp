#include "AppWindow.hpp"

HRESULT AppWindow::Create(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
	wcex.hInstance = hInstance;
	wcex.lpfnWndProc = WndProc;
	wcex.lpszClassName = ClassName;
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassExW(&wcex);

	this->hWnd = CreateWindowW(ClassName, Title,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, Width, Height,
		nullptr, nullptr, hInstance, this);
	if (this->hWnd == nullptr) {
		return E_FAIL;
	}

	return this->app.Initialize(this->hWnd);
}

void AppWindow::Show(int nCmdShow)
{
	ShowWindow(this->hWnd, nCmdShow);
	UpdateWindow(this->hWnd);
}

void AppWindow::RunMessageLoop()
{
	MSG msg;
	while (GetMessageW(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
}

LRESULT CALLBACK AppWindow::WndProc(HWND hWnd, UINT uMsg,
	WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_CREATE) {
		auto pcs = (LPCREATESTRUCT)lParam;
		auto pWindow = (AppWindow*)pcs->lpCreateParams;
		SetWindowLongPtrW(hWnd, GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(pWindow));
		return 0;
	}

	auto pWindow = reinterpret_cast<AppWindow*>(static_cast<LONG_PTR>(
		GetWindowLongPtrW(hWnd, GWLP_USERDATA)));

	if (pWindow != nullptr) {
		switch (uMsg) {
		case WM_PAINT: {
			auto hr = pWindow->app.Render();
			if (SUCCEEDED(hr)) {
				ValidateRect(hWnd, nullptr);
			}
			return 0;
		}

		case WM_KEYDOWN: {
			bool handled = pWindow->app.SendKey(wParam);
			if (handled) {
				InvalidateRect(hWnd, nullptr, FALSE);
				return 0;
			}
			break;
		}

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
	}

	return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}
