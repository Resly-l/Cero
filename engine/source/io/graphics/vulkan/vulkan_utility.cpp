#include "vulkan_utility.h"
#include "vulkan_validation.hpp"

namespace io::graphics
{
	VkFormat VkTypeConverter::Convert(ImageFormat _format)
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

	VkImageUsageFlags VkTypeConverter::Convert(ImageUsage _usage)
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

	VkImageAspectFlags VkTypeConverter::GetAspectMask(VkFormat _format, VkImageUsageFlags _usage)
	{
		VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_NONE;

		if (_usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
		{
			aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}
		else if (_usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			if (_format >= VK_FORMAT_D16_UNORM_S8_UINT)
			{
				aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}

		return aspectMask;
	}

	VkAttachmentLoadOp VkTypeConverter::ConvertLoadOp(ImageOperation _operation)
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

    VkAttachmentStoreOp VkTypeConverter::ConvertStoreOp(ImageOperation _operation)
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

	VkDescriptorType VkTypeConverter::Convert(ShaderBinding::Type _type)
	{
		switch (_type)
		{
		case ShaderBinding::Type::TEXTURE:
			return VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		case ShaderBinding::Type::UNIFORM:
			return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		}

		return VkDescriptorType{};
	}

	VkShaderStageFlags VkTypeConverter::Convert(ShaderBinding::Stage _stage)
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