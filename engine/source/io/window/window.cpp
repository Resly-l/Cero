#include "window.h"

namespace io::window
{
	Window::Window(uint32_t _width, uint32_t _height, std::string_view _title, std::string_view _className)
		: className_(_className)
		, title_(_title)
		, instance_(GetModuleHandle(nullptr))
	{
		RegisterWindowClass(_title, std::string_view{});

		Instantiate(_width, _height);
		SetVisibility(true);
	}

	Window::~Window()
	{
		DestroyWindow(wnd_);
		UnregisterClassA(className_.c_str(), instance_);
	}

	bool Window::ProcessMessage()
	{
		MSG msg = {};
		while (PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				return false;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return true;
	}

	std::string_view Window::GetTitle() const
	{
		return title_;
	}

	void Window::SetTitle(std::string_view _title)
	{
		title_ = _title;
		SetWindowTextA(wnd_, title_.c_str());
	}

	bool Window::SetVisibility(bool _visible)
	{
		return ShowWindow(wnd_, _visible ? SW_SHOW : SW_HIDE);
	}

	void Window::Close() const
	{
		PostQuitMessage(0);
	}

	void Window::RegisterWindowClass(std::string_view _className, std::string_view _iconPath) const
	{
		WNDCLASSEXA wc = {};
		wc.cbSize = sizeof(WNDCLASSEXA);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WindowProc;
		wc.hInstance = instance_;
		wc.hIcon = (HICON)LoadImageA(0, _iconPath.data(), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.lpszClassName = className_.c_str();

		RegisterClassExA(&wc);
	}

	void Window::Instantiate(uint32_t _width, uint32_t _height)
	{
		RECT windowRect = { 0, 0, (LONG)_width, (LONG)_height };
		AdjustWindowRect(&windowRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

		wnd_ = CreateWindowExA(0, className_.c_str(), title_.c_str(), WS_SYSMENU | WS_MINIMIZEBOX,
			GetSystemMetrics(SM_CXSCREEN) / 2 - (windowRect.right - windowRect.left) / 2,
			GetSystemMetrics(SM_CYSCREEN) / 2 - (windowRect.bottom - windowRect.top) / 2,
			windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
			nullptr, nullptr, instance_, nullptr
		);
	}

	LRESULT Window::WindowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_CLOSE:
			PostQuitMessage(S_OK);
			return S_OK;
		}

		return DefWindowProcA(window, msg, wParam, lParam);
	}
}