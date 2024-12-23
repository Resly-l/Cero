#pragma once
#include "pipeline.h"
#include "mesh.h"
#include "uniform_buffer.h"
#include "texture.h"
#include "render_target.h"

namespace graphics
{
	class GraphicsAPI
	{
	public:
		enum Type
		{
			VULKAN,
		};

		struct Config
		{
			uint32_t numFrameConcurrency_ = 2;// experimental value
			
			// todo : make actual dynamic pool
			uint32_t numMaxDescriptorSets_ = 128;
			uint32_t numMaxSamplers_ = 128;
			uint32_t numMaxUBuffers_ = 128;
		};

	protected:
		Config config_;

	public:
		virtual ~GraphicsAPI() = default;

	public:
		Config GetConfig() const { return config_; }

		virtual std::shared_ptr<Pipeline> CreatePipeline(const Pipeline::Layout& _pipelineLayout) = 0;
		virtual std::shared_ptr<Mesh> CreateMesh(const Mesh::Layout& _meshLayout) = 0;
		virtual std::shared_ptr<Material> CreateMaterial() = 0;
		virtual std::shared_ptr<UniformBuffer> CreateUniformBuffer(const UniformBuffer::Layout& _layout) = 0;
		virtual std::shared_ptr<Texture> CreateTexture(const Texture::Layout& _textureLayout) = 0;
		virtual std::shared_ptr<RenderPass> CreateRenderPass() = 0;

		virtual uint32_t GetCurrentFrameIndex() const = 0;
		virtual std::shared_ptr<RenderTarget> GetSwapchainRenderTarget() const = 0;
		virtual bool WaitSwapchainImage() = 0;
		virtual void Present() = 0;
		virtual void Resize(uint32_t _width, uint32_t _height) {};
		virtual void WaitIdle() = 0;
	};
}