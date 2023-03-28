#pragma once
#include "WindowsMinimal.h"
#include <string>
#include "Renderer/Renderer.h"


class Window
{
private:
	HWND window;
	HINSTANCE instance;

	inline static const std::wstring className = L"Window Class";
	std::wstring title;

protected:
	std::unique_ptr<Renderer> renderer;

public:
	Window(int width, int height, std::wstring_view title_in, std::wstring_view iconPath = L"");
	virtual ~Window();

public:
	bool ChangeTitle(std::wstring_view title_in);
	bool ProcessMessage() const;
	void Close() const;

private:
	void RegisterWindowClass(HINSTANCE instance, LPCWSTR iconPath) const;
	static LRESULT WindowProc(HWND, UINT, WPARAM, LPARAM);
};