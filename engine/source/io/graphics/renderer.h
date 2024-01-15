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
		void BeginFrame();
		std::shared_ptr<Pipeline> CreatePipeline(const PipelineState& _pipelineState);
		void BindPipeline(std::shared_ptr<Pipeline> _pipeline);
		void Render();
		void EndFrame();
	};
}