#include "window.h"

namespace window
{
	Window::Window(uint32_t _width, uint32_t _height, std::string _title, std::string _className)
		: className_(std::move(_className))
		, title_(std::move(_title))
		, instance_(GetModuleHandle(nullptr))
	{
		RegisterWindowClass(std::string_view{});
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

	void Window::SetTitle(std::string _title)
	{
		title_ = std::move(_title);
		SetWindowTextA(wnd_, title_.c_str());
	}

	bool Window::SetVisibility(bool _visible)
	{
		return ShowWindow(wnd_, _visible ? SW_SHOW : SW_HIDE);
	}

	void Window::SetResizability(bool _resizable)
	{
		DWORD windowStyle = (DWORD)GetWindowLongPtrA(wnd_, GWL_STYLE);

		if (_resizable)
		{
			SetWindowLongPtrA(wnd_, GWL_STYLE, windowStyle | WS_THICKFRAME | WS_MAXIMIZEBOX);
		}
		else
		{
			SetWindowLongPtrA(wnd_, GWL_STYLE, (windowStyle & ~WS_THICKFRAME) & ~WS_MAXIMIZEBOX);
		}
	}

	void Window::Close() const
	{
		PostQuitMessage(0);
	}

	bool Window::IsMinimized() const
	{
		return IsIconic(wnd_);
	}

	void Window::RegisterWindowClass(std::string_view _iconPath) const
	{
		WNDCLASSEXA wc = {};
		wc.cbSize = sizeof(WNDCLASSEXA);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = SetupWindowProc;
		wc.hInstance = instance_;
		wc.hIcon = (HICON)LoadImageA(0, _iconPath.data(), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.lpszClassName = className_.c_str();

		RegisterClassExA(&wc);
	}

	void Window::Instantiate(uint32_t _width, uint32_t _height)
	{
		DWORD windowStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_OVERLAPPEDWINDOW;

		RECT windowRect = { 0, 0, (LONG)_width, (LONG)_height };
		AdjustWindowRect(&windowRect,windowStyle, FALSE);

		wnd_ = CreateWindowExA(0, className_.c_str(), title_.c_str(), windowStyle,
			GetSystemMetrics(SM_CXSCREEN) / 2 - (windowRect.right - windowRect.left) / 2,
			GetSystemMetrics(SM_CYSCREEN) / 2 - (windowRect.bottom - windowRect.top) / 2,
			windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
			nullptr, nullptr, instance_, this
		);
	}

	LRESULT Window::SetupWindowProc(HWND _window, UINT _msg, WPARAM _wParam, LPARAM _lParam)
	{
		if (_msg == WM_NCCREATE)
		{
			CREATESTRUCTW* createStruct = reinterpret_cast<CREATESTRUCTW*>(_lParam);
			Window* window = createStruct ? static_cast<Window*>(createStruct->lpCreateParams) : nullptr;
			if (window == nullptr)
			{
				throw std::exception("failed to setup window proc");
			}

			SetWindowLongPtrA(_window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
			SetWindowLongPtrA(_window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::WindowProc));

			return window->HandleMessage(_window, _msg, _wParam, _lParam);
		}

		return DefWindowProcA(_window, _msg, _wParam, _lParam);
	}

	LRESULT Window::WindowProc(HWND _window, UINT _msg, WPARAM _wParam, LPARAM _lParam)
	{
		Window* window = reinterpret_cast<Window*>(GetWindowLongPtrA(_window, GWLP_USERDATA));
		if (window == nullptr)
		{
			return S_FALSE;
		}

		return window->HandleMessage(_window, _msg, _wParam, _lParam);
	}

	LRESULT Window::HandleMessage(HWND _window, UINT _msg, WPARAM _wParam, LPARAM _lParam)
	{
		uint32_t highWord = HIWORD(_lParam);
		uint32_t lowWord = LOWORD(_lParam);

		switch (_msg)
		{
		case WM_SIZE:
			Resize(lowWord, highWord);
			break;
		case WM_CLOSE:
			PostQuitMessage(S_OK);
			return S_OK;
		}

		return DefWindowProcA(_window, _msg, _wParam, _lParam);
	}
}