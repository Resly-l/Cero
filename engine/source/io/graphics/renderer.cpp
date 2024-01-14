#include "renderer.h"
#include "private/vulkan_api.h"

namespace io::graphics
{
	Renderer::Renderer(GLFWwindow& _window)
		: graphicsAPI_(std::make_unique<VulkanAPI>(_window))
	{
	}

	void Renderer::BeginFrame()
	{
	}

	void Renderer::SetPipelineState(std::shared_ptr<PipelineState> _pipelineState)
	{
		pipelineState_ = _pipelineState;
		return graphicsAPI_->BindPipeline(*_pipelineState);
	}

	void Renderer::Render()
	{
		if (!pipelineState_)
		{
			return;
		}

		graphicsAPI_->Draw();
	}

	void Renderer::EndFrame()
	{
		graphicsAPI_->Present();
	}
}