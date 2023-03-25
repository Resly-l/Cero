#include "Window.h"

Window::Window(int width_in, int height_in, std::wstring_view title_in, std::wstring_view iconPath)
	:
	instance(GetModuleHandleW(nullptr)),
	title(title_in),
	width(width_in),
	height(height_in)
{
	RegisterWindowClass(instance, iconPath.data());
	window = CreateWindowExW(0, className.c_str(), title.data(), WS_SYSMENU | WS_MINIMIZEBOX, 0, 0, width, height, nullptr, nullptr, instance, nullptr);

	renderer = std::make_unique<Renderer>(window, width, height);

	ShowWindow(window, SW_SHOW);
}

Window::~Window()
{
	DestroyWindow(window);
}

bool Window::ChangeTitle(std::wstring_view title_in)
{
	return SetWindowTextW(window, title_in.data());
}

bool Window::ProcessMessage() const
{
	MSG msg = {};
	while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return false;
		}

		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return true;
}

void Window::Close() const
{
	PostQuitMessage(0);
}

void Window::RegisterWindowClass(HINSTANCE instance, LPCWSTR iconPath) const
{
	WNDCLASSEXW wc = {};

	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = instance;
	wc.hIcon = (HICON)LoadImageW(0, iconPath, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.lpszClassName = L"Window Class";

	RegisterClassExW(&wc);
}

LRESULT Window::WindowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(S_OK);
		return S_OK;
	}

	return DefWindowProcW(window, msg, wParam, lParam);
}
