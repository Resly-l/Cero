#pragma once
#include "Common/Types.h"
#include "Renderer/Renderer.h"


struct GLFWwindow;

class Window
{
private:
	GLFWwindow* window = nullptr;
	std::string title;

protected:
	std::unique_ptr<Renderer> renderer;

public:
	Window(int width, int height, std::string_view title_in, std::string_view iconPath = "");
	virtual ~Window();

public:
	virtual void Run();
	void Close() const;

protected:
	bool ProcessMessage();

private:
	static LRESULT WindowProc(HWND, UINT, WPARAM, LPARAM);
};