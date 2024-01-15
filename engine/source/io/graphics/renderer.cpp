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
		graphicsAPI_->BeginFrame();
	}

	std::shared_ptr<Pipeline> Renderer::CreatePipeline(const PipelineState& _pipelineState)
	{
		return graphicsAPI_->CreatePipeline(_pipelineState);
	}

	void Renderer::BindPipeline(std::shared_ptr<Pipeline> _pipeline)
	{
		graphicsAPI_->BindPipeline(_pipeline);
	}

	void Renderer::Render()
	{
		graphicsAPI_->Draw();
	}

	void Renderer::EndFrame()
	{
		graphicsAPI_->EndFrame();
	}
}