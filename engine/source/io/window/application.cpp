#include "application.h"

namespace io::window
{
	Application::Application()
		: Window(defaultWindowWidth, defaultWindowHeight, "Window Application")
	{
		renderer_ = std::make_unique<graphics::Renderer>(*window_);
	}

	void Application::Run()
	{
		while (Window::ProcessMessage())
		{
			renderer_->BeginFrame();
			Update();
			renderer_->EndFrame();
		}
	}
}