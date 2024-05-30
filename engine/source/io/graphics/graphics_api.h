#pragma once
#include "pipeline.h"
#include "render_target.h"
#include "mesh.h"

namespace io::graphics
{
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
		virtual std::shared_ptr<RenderTarget> CreateRenderTarget(const RenderTarget::Layout& _renderTargetLayout) = 0;
		virtual std::shared_ptr<Mesh> CreateMesh(const Mesh::Layout& _meshLayout) = 0;

		virtual std::shared_ptr<RenderTarget> GetSwapchainRenderTarget() const = 0;

		virtual void BindPipeline(std::shared_ptr<Pipeline> _pipeline) = 0;
		virtual void BindRenderTarget(std::shared_ptr<RenderTarget> _renderTarget) = 0;
		virtual void BindMesh(std::shared_ptr<Mesh> _mesh) = 0;

		virtual bool BeginFrame() = 0;
		virtual void Draw() = 0;
		virtual void EndFrame() = 0;

		virtual void Resize(uint32_t _width, uint32_t _height) = 0;
		virtual void WaitIdle() = 0;
	};
}