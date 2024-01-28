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
		std::shared_ptr<Pipeline> CreatePipeline(const PipelineState& _pipelineState);

		void BeginFrame();
		void Render(std::shared_ptr<Pipeline> _pipeline);
		void EndFrame();
	};
}