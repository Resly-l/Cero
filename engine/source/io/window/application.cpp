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
			renderer_->GetGraphicsAPI().BeginFrame();
			Update();
			renderer_->GetGraphicsAPI().EndFrame();
		}
	}
}