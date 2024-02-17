#pragma once
#include "io/graphics/graphics_api.h"
#include "vulkan/vulkan.h"

namespace io::graphics
{
	class VulkanAttachment;

	class VulkanRenderPass : public RenderPass
	{
	private:
		const VkDevice& logicalDevice_;
		VkRenderPass instance_;
		VkCommandBuffer commandBuffer_;
		VkSemaphore semaphore_;
		VkFence fence_;

		std::vector<std::unique_ptr<VulkanAttachment>> framebufferAttachments_;
		uint32_t numColorAttachments_ = 0;
		VkFramebuffer framebuffer_;
		uint32_t framebufferWidth_;
		uint32_t framebufferHeight_;

	public:
		VulkanRenderPass(const VkDevice& _logicalDevice, VkCommandPool _commandPool, const RenderPassLayout& _renderPassLayout, uint32_t _width, uint32_t _height);
		~VulkanRenderPass();

	public:
		VkRenderPass GetInstance() const;
		VkCommandBuffer GetCommandBuffer() const;
		VkSemaphore GetSemaphore() const;
		VkFence GetFence() const;
		uint32_t GetNumColorAttachements() const;
		uint32_t GetNumFramebufferAttachments() const;
		VkFramebuffer GetFramebuffer() const;
		uint32_t GetFramebufferWidth() const;
		uint32_t GetFramebufferHeight() const;
	};
}