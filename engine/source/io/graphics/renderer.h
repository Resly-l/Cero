#pragma once
#include "utility/stl.h"
#include "graphics_api.h"

struct GLFWwindow;

namespace io::graphics
{
	class Renderer
	{
	private:
		std::unique_ptr<GraphicsAPI> graphicsAPI_;

	public:
		Renderer(GLFWwindow& _window);

	public:
		GraphicsAPI& GetGraphicsAPI();
	};
}