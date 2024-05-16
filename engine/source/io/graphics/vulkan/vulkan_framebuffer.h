#pragma once
#include "io/graphics/graphics_api.h"
#include <vulkan/vulkan.h>

namespace io::graphics
{
		struct VulkanAttachment
	{
		VkImage image_ = VK_NULL_HANDLE;
		VkImageView imageView_ = VK_NULL_HANDLE;
		VkDeviceMemory memory_ = VK_NULL_HANDLE;
	};

	class VulkanFramebuffer
	{
	private:
		VkDevice logicalDevice_;
		VkFramebuffer instance_;
		uint32_t width_;
		uint32_t height_;
		std::vector<VulkanAttachment>  attachments_;

	public:
		VulkanFramebuffer(VkDevice _logicalDevice, VkRenderPass _renderPass, const Framebuffer::Layout& _framebufferLayout);
		~VulkanFramebuffer();

	public:
		VkFramebuffer GetInstance() const;
		uint32_t GetNumAttachements(bool _excludeDepthAttachement = true) const;

	private:
		void InitializeAttachments(VkDevice _logicalDevice,  const Framebuffer::Layout& _framebufferLayout);
		void CreateInstance(VkDevice _logicalDevice, VkRenderPass _renderPass);
	};
}