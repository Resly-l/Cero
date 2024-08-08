#include "vulkan_utility.h"
#include "vulkan_result.hpp"

namespace io::graphics
{
	VkFormat VulkanTypeConverter::Convert(ImageFormat _format)
	{
		switch (_format)
		{
		case ImageFormat::R8G8B8_UNORM:
			return VK_FORMAT_R8G8B8_UNORM;
		case ImageFormat::R8G8B8_NORM:
			return VK_FORMAT_R8G8B8_SNORM;
		case ImageFormat::R8G8B8A8_UNORM:
			return VK_FORMAT_R8G8B8A8_UNORM;
		case ImageFormat::R8G8B8A8_NORM:
			return VK_FORMAT_R8G8B8A8_SNORM;
		case ImageFormat::B8G8R8_UNORM:
			return VK_FORMAT_B8G8R8_UNORM;
		case ImageFormat::B8G8R8_NORM:
			return VK_FORMAT_B8G8R8_SNORM;
		case ImageFormat::B8G8R8A8_UNORM:
			return VK_FORMAT_B8G8R8A8_UNORM;
		case ImageFormat::B8G8R8A8_NORM:
			return VK_FORMAT_B8G8R8A8_SNORM;
			case ImageFormat::D32_SFLOAT:
			return VK_FORMAT_D32_SFLOAT;
		case ImageFormat::D32_SFLOAT_U8_UINT:
			return VK_FORMAT_D32_SFLOAT_S8_UINT;
		}

		return VK_FORMAT_UNDEFINED;
	}

	VkImageUsageFlags VulkanTypeConverter::Convert(ImageUsage _usage)
	{
		switch (_usage)
		{
		case ImageUsage::COLOR_ATTACHMENT:
			return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		case ImageUsage::DEPTH_STENCIL:
			return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		}

		return VkImageUsageFlags{};
	}

	VkAttachmentLoadOp VulkanTypeConverter::ConvertLoadOp(ImageOperation _operation)
	{
		switch (_operation)
		{
		case ImageOperation::STORE:
			return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_LOAD;
		case ImageOperation::CLEAR:
			return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
		}

		return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	}

    VkAttachmentStoreOp VulkanTypeConverter::ConvertStoreOp(ImageOperation _operation)
    {
        switch (_operation)
		{
		case ImageOperation::STORE:
			return VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
		case ImageOperation::CLEAR:
			return VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
		}

		return VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
    }

	VkShaderStageFlags VulkanTypeConverter::Convert(ShaderBinding::Stage _stage)
	{
		VkShaderStageFlags flags{};
		if (_stage & ShaderBinding::Stage::VERTEX)
		{
			flags |= VK_SHADER_STAGE_VERTEX_BIT;
		}
		if (_stage & ShaderBinding::Stage::PIXEL)
		{
			flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
		}

		return flags;
	}
}