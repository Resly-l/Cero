#pragma once
#include "pipeline.h"
#include "model.h"
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
			uint32_t numFrameConcurrency_ = 2;
			
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
		virtual std::shared_ptr<Pipeline> CreatePipeline(const Pipeline::Layout& _pipelineLayout) = 0;
		virtual std::shared_ptr<Mesh> CreateMesh(const Mesh::Layout& _meshLayout) = 0;
		virtual std::shared_ptr<Material> CreateMaterial(const Material::Layout& _materialLayout) = 0;
		virtual std::shared_ptr<Model> CreateModel(const Model::Layout& _modelLayout) = 0;
		virtual std::shared_ptr<UniformBuffer> CreateUniformBuffer(const UniformBuffer::Layout& _layout) = 0;
		virtual std::shared_ptr<Texture> CreateTexture(const Texture::Layout& _textureLayout) = 0;
		virtual std::shared_ptr<RenderTarget> GetSwapchainRenderTarget() = 0;

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