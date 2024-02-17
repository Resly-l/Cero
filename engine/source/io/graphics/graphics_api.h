#pragma once
#include "pipeline_state.h"

namespace io::graphics
{
	class DeviceResource
	{
	public:
		~DeviceResource() = default;
	};

	class RenderPass : public DeviceResource {};
	class Pipeline : public DeviceResource {};

	enum class ImageFormat
	{
		ifB8G8R8A8_UNORM,
		ifD32_SFLOAT,
	};

	enum class ImageUsage
	{
		iuCOLOR_ATTACHMENT,
		iuDEPTH_STENCIL_ATTACHMENT,
	};

	enum class ImageOperation
	{
		ioNONE,
		ioCLEAR,
		ioLOAD,
		ioSTORE,
	};

	enum class ImageLayout
	{
		ilUNDEFINED,
		ilPRESENT_SOURCE,
		ilDEPTH_STENCIL,
		ilSHADER_READ,
	};

	struct AttachmentDescription
	{
		ImageFormat format_;
		ImageUsage usage_;
		ImageOperation loadOperation_;
		ImageOperation storeOperation_;
		ImageOperation stencilLoadOperation_;
		ImageOperation stencilStoreOperation_;
		ImageLayout initialLayout_;
		ImageLayout finalLayout_;
		uint32_t width_;
		uint32_t height_;
	};

	struct SubpassDescription
	{
		uint32_t numColorReferences_;
	};

	struct DependencyDescription
	{
		bool source_;
	};

	struct RenderPassLayout
	{
		std::vector<AttachmentDescription> attachmentDescriptions_;
		std::vector<SubpassDescription> subpassDescriptions_;
		std::vector<DependencyDescription> dependencyDescriptions_;
	};

	class GraphicsAPI
	{
	protected:
		std::vector<std::shared_ptr<DeviceResource>> deviceResources_;

	public:
		virtual ~GraphicsAPI() = default;

	public:
		virtual std::shared_ptr<RenderPass> CreateRenderPass(const RenderPassLayout& _renderPassLayout, uint32_t _width, uint32_t _height) = 0;
		virtual std::shared_ptr<Pipeline> CreatePipeline(const PipelineState& _pipelineState, const RenderPass& _renderPass) = 0;
		virtual void BindRenderPass(std::shared_ptr<RenderPass> _renderPass) = 0;
		virtual void BindPipeline(std::shared_ptr<Pipeline> _pipeline) = 0;

		virtual void BeginFrame() = 0;
		virtual void Draw() = 0;
		virtual void EndFrame() = 0;
	};
}