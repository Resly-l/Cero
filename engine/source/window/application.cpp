#include "application.h"
#include "graphics/vulkan/vulkan_api.h"
#include "thread/thread_pool.h"

namespace window
{
	Application::Application(graphics::GraphicsAPI::Type _apiType)
		: Window(defaultWindowWidth, defaultWindowHeight, "Window Application")
	{
		thread::ThreadPool::Initialize();

		if (_apiType == graphics::GraphicsAPI::Type::VULKAN)
		{
			graphicsAPI_ = std::make_unique<graphics::VulkanAPI>(wnd_);
		}
		else
		{
			throw std::exception("invalid graphics api type");
		}

		renderer_ = std::make_unique<graphics::Renderer>();
	}

	void Application::Run()
	{
		while (Window::ProcessMessage())
		{
			bool frameBegun = false;

			if (IsMinimized() == false)
			{
				frameBegun = graphicsAPI_->WaitSwapchainImage();
			}

			Update();

			if (frameBegun)
			{
				renderer_->RenderFrame(*graphicsAPI_);
				graphicsAPI_->Present();
			}
		}

		thread::ThreadPool::Deinitialize();
		graphicsAPI_->WaitIdle();
	}

	void Application::Resize(uint32_t _width, uint32_t _height)
	{
		graphicsAPI_->Resize(_width, _height);
	}
}