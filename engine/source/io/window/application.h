#pragma once
#include "io/window/window.h"
#include "io/graphics/renderer.h"

namespace io::window
{
	class Application : public Window
	{
	private:
		static constexpr uint32_t defaultWindowWidth = 1600;
		static constexpr uint32_t defaultWindowHeight = 900;

	protected:
		std::unique_ptr<graphics::Renderer> renderer_;

	public:
		Application();
		virtual ~Application() = default;

	public:
		void Run();
	};
}