#include "vulkan_attachment.h"
#include "vulkan_bootstrap.h"

namespace io::graphics
{
	VulkanAttachment::VulkanAttachment(const VkDevice& _logicalDevice, VkSurfaceFormatKHR _format, VkImageUsageFlagBits _usage, uint32_t _width, uint32_t _height, uint32_t _depth)
		: logicalDevice_(_logicalDevice)
	{
		image_ = VulkanBootstrap::CreateImage(logicalDevice_, _format, _usage, _width, _height, _depth);
		imageMemory_ = VulkanBootstrap::AllocateImageMemory(logicalDevice_, image_);
		imageView_ = VulkanBootstrap::CreateImageView(logicalDevice_, image_, _format, _usage);
	}

	VulkanAttachment::~VulkanAttachment()
	{
		vkDestroyImageView(logicalDevice_, imageView_, nullptr);
		vkFreeMemory(logicalDevice_, imageMemory_, nullptr);
		vkDestroyImage(logicalDevice_, image_, nullptr);
	}

	VkImageView VulkanAttachment::GetImageView() const
	{
		return imageView_;
	}
}