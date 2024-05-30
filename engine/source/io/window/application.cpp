#include "application.h"

#include "io/graphics/vulkan/vulkan_api.h"

namespace io::window
{
	Application::Application(graphics::GraphicsAPIType _apiType)
		: Window(defaultWindowWidth, defaultWindowHeight, "Window Application")
	{
		switch (_apiType)
		{
		case graphics::GraphicsAPIType::Vulkan:
			graphicsAPI_ = std::make_unique<graphics::VulkanAPI>(wnd_);
			return;
		default:
			throw std::exception("invalid graphics api type");
		}
	}

	void Application::Run()
	{
		while (Window::ProcessMessage())
		{
			bool minimized = IsMinimized();
			bool frameBegun = false;

			if (!minimized)
			{
				frameBegun = graphicsAPI_->BeginFrame();
			}

			Update();

			if (!minimized && frameBegun)
			{
				Render();
				graphicsAPI_->EndFrame();
			}
		}
	}

	void Application::Resize(uint32_t _width, uint32_t _height)
	{
		graphicsAPI_->Resize(_width, _height);
	}
}