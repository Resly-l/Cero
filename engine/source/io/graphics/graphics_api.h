#pragma once
#include "graphics_pipeline.h"

namespace io::graphics
{
	class RenderTarget {};
	struct RenderTargetLayout {};

	enum class GraphicsAPIType
	{
		Vulkan,
	};

	class GraphicsAPI
	{
	public:
		struct Config
		{
			uint32_t numFrameConcurrency_ = 2;
		};

	protected:
		Config config_;

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

		virtual void Resize(uint32_t _width, uint32_t _height) = 0;
	};
}