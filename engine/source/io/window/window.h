#pragma once
#include "utility/stl.h"
#include "window_min.h"

struct GLFWwindow;

namespace io::window
{
	class Window
	{
	private:
		std::string title_;

	protected:
		GLFWwindow* window_ = nullptr;

	public:
		Window(int _width, int _height, std::string_view _title);
		virtual ~Window();

	protected:
		bool ProcessMessage();
		virtual void Update() = 0;
		void Close() const;

		std::string_view GetTitle() const;
		void SetTitle(std::string_view _title);

	private:
		static LRESULT WindowProc(HWND, UINT, WPARAM, LPARAM);
	};
}