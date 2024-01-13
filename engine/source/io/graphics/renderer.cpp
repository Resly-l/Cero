#include "renderer.h"
#include "private/vulkan.h"

namespace io::graphics
{
	Renderer::Renderer(GLFWwindow& _window)
		: graphicsAPI_(std::make_unique<Vulkan>(_window))
	{
	}

	void Renderer::BeginFrame()
	{
	}

	void Renderer::SetPipelineState(std::shared_ptr<PipelineState> _pipelineState)
	{
#if _DEBUG
		pipelineState_ = _pipelineState;
#endif
		return graphicsAPI_->BindPipeline(*_pipelineState);
	}

	void Renderer::Render()
	{
#if _DEBUG
		if (!pipelineState_)
		{
			return;
		}
#endif

		graphicsAPI_->Execute();
	}

	void Renderer::EndFrame()
	{
#if _DEBUG
		pipelineState_.reset();
#endif
	}
}