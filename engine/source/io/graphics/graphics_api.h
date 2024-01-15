#pragma once
#include "pipeline_state.h"

namespace io::graphics
{
	class Pipeline
	{

	};

	class GraphicsAPI
	{
	public:
		virtual ~GraphicsAPI() = default;

	public:
		virtual void BeginFrame() = 0;
		virtual std::shared_ptr<Pipeline> CreatePipeline(const PipelineState& _pipelineState) = 0;
		virtual void BindPipeline(std::shared_ptr<Pipeline> _pipeline) = 0;
		virtual void Draw() = 0;
		virtual void EndFrame() = 0;
	};
}