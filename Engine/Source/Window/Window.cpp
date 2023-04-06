#include "Window.h"
#include "ThirdParty/glfw/glfw3.h"
#include <iostream>

Window::Window(int width, int height, std::string_view title_in, std::string_view iconPath)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(width, height, title_in.data(), nullptr, nullptr);

	renderer = std::make_unique<Renderer>(window, width, height);
}

Window::~Window()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

bool Window::ProcessMessage()
{
	glfwPollEvents();
	return !glfwWindowShouldClose(window);
}

void Window::Run()
{
	while (ProcessMessage())
	{
		renderer->BeginFrame();
		renderer->Render();
		renderer->EndFrame();
	}
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
