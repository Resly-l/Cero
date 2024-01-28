#include "window.h"
#include "thirdparty/glfw/glfw3.h"

namespace io::window
{
	Window::Window(int _width, int _height, std::string_view _title)
		: title_(_title)
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

		window_ = glfwCreateWindow(_width, _height, _title.data(), nullptr , nullptr);
	}

	Window::~Window()
	{
		glfwDestroyWindow(window_);
		glfwTerminate();
	}

	bool Window::ProcessMessage()
	{
		glfwPollEvents();
		return !glfwWindowShouldClose(window_);
	}

	std::string_view Window::GetTitle() const
	{
		return title_;
	}

	void Window::SetTitle(std::string_view _title)
	{
		title_ = _title;
		glfwSetWindowTitle(window_, title_.c_str());
	}

	void Window::Close() const
	{
		PostQuitMessage(0);
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
}