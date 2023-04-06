#include "Renderer.h"
#include "Private/VulkanAPI.h"


Renderer::Renderer(GLFWwindow* window, int width, int height)
	:
	graphicsAPI(std::make_unique<VulkanAPI>(window, width, height))
{
}

void Renderer::BeginFrame()
{
}

void Renderer::Render()
{
	((VulkanAPI*)(graphicsAPI.get()))->Execute();
	
	if (!pipelineState || !pipelineState->IsComplete())
	{
		return;
	}


}

void Renderer::EndFrame()
{
}

bool Renderer::SetPipelineState(std::shared_ptr<PipelineState> pipelineState_in)
{
	if (!pipelineState_in || !pipelineState_in->IsComplete())
	{
		return false;
	}

	pipelineState = pipelineState_in;
	return true;
}
