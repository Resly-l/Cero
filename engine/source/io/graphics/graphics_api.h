#pragma once
#include "pipeline_state.h"

namespace io::graphics
{
	class GraphicsAPI
	{
	public:
		virtual ~GraphicsAPI() = default;

	public:
		virtual void BindPipeline(const PipelineState& _pipelineState) = 0;
		virtual void Draw() = 0;
		virtual void Present() = 0;
	};
}