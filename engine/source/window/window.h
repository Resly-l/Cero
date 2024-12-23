#pragma once
#include "window_min.h"
#include <string>

namespace window
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
		Window(uint32_t _width, uint32_t _height, std::string _title, std::string _className = "default class name");
		virtual ~Window();

		std::string_view GetTitle() const;
		void SetTitle(std::string _title);
		bool SetVisibility(bool _visible);
		void SetResizability(bool _resizable);
		void Close() const;
		bool IsMinimized() const;
		virtual void Resize(uint32_t _width, uint32_t _height) {};

	protected:
		bool ProcessMessage();
		virtual void Tick() = 0;

	private:
		void RegisterWindowClass(std::string_view _iconPath) const;
		void Instantiate(uint32_t _width, uint32_t _height);

		static LRESULT SetupWindowProc(HWND _window, UINT _msg, WPARAM _wParam, LPARAM _lParam);
		static LRESULT WindowProc(HWND _window, UINT _msg, WPARAM _wParam, LPARAM _lParam);
		LRESULT HandleMessage(HWND _window, UINT _msg, WPARAM _wParam, LPARAM _lParam);
	};
}