#pragma once
#include "graphics_pipeline.h"

namespace io::graphics
{
	class RenderTarget {};
	struct RenderTargetLayout {};

	class GraphicsAPI
	{
	public:
		virtual ~GraphicsAPI() = default;

	public:
		virtual std::shared_ptr<Pipeline> CreatePipeline(const Pipeline::Layout& _pipelineLayout) = 0;
		virtual std::shared_ptr<RenderTarget> CreateRenderTarget(const RenderTargetLayout& _renderTargetLayout) = 0;

		virtual void BindPipeline(std::shared_ptr<Pipeline> _pipeline) = 0;
		virtual void BindRenderTargets(std::vector<std::shared_ptr<RenderTarget>> _renderTargets) = 0;

		virtual void BeginFrame() = 0;
		virtual void Draw() = 0;
		virtual void EndFrame() = 0;
	};

	enum class GraphicsAPIType
	{
		Vulkan,
	};
}