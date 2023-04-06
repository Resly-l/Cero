#pragma once
#include "IGraphicsAPI.h"
#include "PipelineState.h"


struct GLFWwindow;

class Renderer
{
private:
	std::unique_ptr<IGraphicsAPI> graphicsAPI;
	std::shared_ptr<PipelineState> pipelineState;

public:
	Renderer(GLFWwindow* window, int width, int height);

public:
	void BeginFrame();
	void Render();
	void EndFrame();

	bool SetPipelineState(std::shared_ptr<PipelineState> pipelineState_in);
};