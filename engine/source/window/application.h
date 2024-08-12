#pragma once
#include "window/window.h"
#include "graphics/graphics_api.h"

namespace window
{
	class Application : public Window
	{
	private:
		static constexpr uint32_t defaultWindowWidth = 1600;
		static constexpr uint32_t defaultWindowHeight = 900;

	protected:
		std::unique_ptr<graphics::GraphicsAPI> graphicsAPI_;

	public:
		Application(graphics::GraphicsAPI::Type _apiType);

	public:
		void Run();
		virtual void Resize(uint32_t _width, uint32_t _height) override;
	};
}