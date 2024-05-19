#pragma once
#include <vulkan/vulkan.h>
#include "io/graphics/pipeline.h"

namespace io::graphics
{
	class VulkanTypeConverter
	{
	public:
		static VkFormat Convert(ImageFormat _format);
		static VkImageUsageFlags Convert(ImageUsage _usage);
		static VkImageAspectFlags GetAspectMask(VkFormat _format, VkImageUsageFlags _usage);
	};
}