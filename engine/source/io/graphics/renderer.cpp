#include "renderer.h"
#include "io/graphics/vulkan/vulkan_api.h"

namespace io::graphics
{
	Renderer::Renderer(GLFWwindow& _window)
		: graphicsAPI_(std::make_unique<VulkanAPI>(_window))
	{
	}

	GraphicsAPI& Renderer::GetGraphicsAPI()
	{
		return *graphicsAPI_;
	}
}