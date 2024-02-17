#pragma once
#include "io/graphics/graphics_api.h"
#include "vulkan/vulkan.h"

namespace io::graphics
{
		class VulkanAttachment
	{
	private:
		const VkDevice& logicalDevice_;
		VkImage image_;
		VkDeviceMemory imageMemory_;
		VkImageView imageView_;

	public:
		VulkanAttachment(const VkDevice& _logicalDevice, VkSurfaceFormatKHR _format, VkImageUsageFlagBits _usage, uint32_t _width, uint32_t _height, uint32_t _depth);
		~VulkanAttachment();

	public:
		VkImageView GetImageView() const;
	};
}