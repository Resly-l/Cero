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
		static VkAttachmentLoadOp ConvertLoadOp(ImageOperation _operation);
		static VkAttachmentStoreOp ConvertStoreOp(ImageOperation _operation);
		static VkShaderStageFlags Convert(ShaderBinding::Stage _stage);
	};
}