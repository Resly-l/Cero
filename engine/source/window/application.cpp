#include "application.h"
#include "graphics/vulkan/vulkan_api.h"
#include "thread/thread_pool.h"

namespace window
{
	Application::Application(graphics::GraphicsAPI::Type _apiType)
		: Window(defaultWindowWidth, defaultWindowHeight, "Window Application")
	{
		thread::ThreadPool::Initialize();

		switch (_apiType)
		{
		case graphics::GraphicsAPI::Type::VULKAN:
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
			const bool minimized = IsMinimized();
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

		thread::ThreadPool::Deinitialize();
		graphicsAPI_->WaitIdle();
	}

	void Application::Resize(uint32_t _width, uint32_t _height)
	{
		graphicsAPI_->Resize(_width, _height);
	}
}