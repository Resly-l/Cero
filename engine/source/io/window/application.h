#pragma once
#include "io/window/window.h"
#include "io/graphics/graphics_api.h"

namespace io::window
{
	class Application : public Window
	{
	private:
		static constexpr uint32_t defaultWindowWidth = 1600;
		static constexpr uint32_t defaultWindowHeight = 900;

	protected:
		std::unique_ptr<graphics::GraphicsAPI> graphicsAPI_;

	public:
		Application(graphics::GraphicsAPIType _apiType);
		virtual ~Application() = default;

	public:
		void Run();

	protected:
		virtual void Resize(uint32_t _width, uint32_t _height) override;
	};
}