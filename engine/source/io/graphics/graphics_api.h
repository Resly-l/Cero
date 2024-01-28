#pragma once
#include "pipeline_state.h"

namespace io::graphics
{
	class DeviceResource
	{
	public:
		~DeviceResource() = default;
	};

	class Pipeline : public DeviceResource {};
	class RenderTarget  : public DeviceResource {};

	class GraphicsAPI
	{
	public:
		virtual ~GraphicsAPI() = default;

	public:
		virtual std::shared_ptr<Pipeline> CreatePipeline(const PipelineState& _pipelineState) = 0;
		virtual std::shared_ptr<RenderTarget> CreateRenderTarget(const PipelineState& _pipelineState) = 0;

		virtual void BindPipeline(std::shared_ptr<Pipeline> _pipeline) = 0;
		virtual void BindRenderTarget(std::shared_ptr<RenderTarget> _renderTarget) = 0;

		virtual void BeginFrame() = 0;
		virtual void Draw() = 0;
		virtual void EndFrame() = 0;
	};
}