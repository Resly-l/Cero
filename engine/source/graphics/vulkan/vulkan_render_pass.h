#pragma once
#include "graphics/render_pass.h"
#include "vulkan/vulkan.h"

namespace graphics
{
	class VulkanRenderPass : public RenderPass
	{
	private:
		std::vector<VkCommandBuffer> commandBuffers_;
		std::vector<VkDescriptorSet> descriptorSets_;
		std::vector<Drawable> drawables_;

		// for static command buffers
		std::vector<uint8_t> pendingCommandBufferUpdate_; //std::vector bool specialization does not return reference to bool

	public:
		virtual void SetPipeline(std::shared_ptr<Pipeline> _pipeline, GraphicsAPI& _graphicsAPI) override;
		virtual void AddDrawable(Drawable _drawable) override;
		virtual void Execute(GraphicsAPI& _graphicsApi, PassResources& _resources) override;
	};
}