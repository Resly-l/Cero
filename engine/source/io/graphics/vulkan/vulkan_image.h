#pragma once
#include "utility/forward_declaration.h"
#include <vulkan/vulkan.h>

namespace io::graphics
{
	class VulkanImageView : public ImageView
	{
	public:
		VkImageView imageView_;

	public:
		VulkanImageView(VkImageView _imageView) : imageView_(_imageView) {};
	};
}