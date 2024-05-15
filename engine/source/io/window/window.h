#pragma once
#include "utility/stl.h"
#include "window_min.h"

namespace io::window
{
	class Window
	{
	private:
		HINSTANCE instance_;
		const std::string className_;
		std::string title_;

	protected:
		HWND wnd_;

	public:
		Window(uint32_t _width, uint32_t _height, std::string_view _title, std::string_view _className = "default class name");
		virtual ~Window();

		// returns previous visibility
		bool SetVisibility(bool _visible);

	protected:
		bool ProcessMessage();
		virtual void Update() = 0;
		void Close() const;

		std::string_view GetTitle() const;
		void SetTitle(std::string_view _title);

	private:
		void RegisterWindowClass(std::string_view _className, std::string_view _iconPath) const;
		void Instantiate(uint32_t _width, uint32_t _height);

		static LRESULT WindowProc(HWND, UINT, WPARAM, LPARAM);
	};
}