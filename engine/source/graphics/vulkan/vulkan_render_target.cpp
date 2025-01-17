#include "vulkan_render_target.h"
#include "vulkan_result.hpp"
#include "vulkan_utility.h"

namespace graphics
{
	VulkanRenderTarget::VulkanRenderTarget(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, const RenderTarget::Layout& _renderTargetLayout)
		: logicalDevice_(_logicalDevice)
		, width_(_renderTargetLayout.width_)
		, height_(_renderTargetLayout.height_)
	{
		for (const ShaderDescriptor::Output& attachmentDescription : _renderTargetLayout.attachments_)
		{
			AddAttachment(_physicalDevice, attachmentDescription);
		}
	}

	VulkanRenderTarget::VulkanRenderTarget(VkDevice _logicalDevice, uint32_t _width, uint32_t _height, VkImageView _outputImageView)
		: logicalDevice_(_logicalDevice)
		, width_(_width)
		, height_(_height)
	{
		attachments_.push_back({ _outputImageView });
	}

	VulkanRenderTarget::~VulkanRenderTarget()
	{
		for (auto& [renderPass, framebuffer] : framebuffers_)
		{
			vkDestroyFramebuffer(logicalDevice_, framebuffer, nullptr);
		}

		for (VulkanAttachment& attachment : attachments_)
		{
			vkDestroyImageView(logicalDevice_, attachment.imageView_, nullptr);

			if (attachment.image_.has_value())
			{
				vkFreeMemory(logicalDevice_, *attachment.memory_, nullptr);
				vkDestroyImage(logicalDevice_, *attachment.image_, nullptr);
			}
		}
	}

	void VulkanRenderTarget::AddAttachment(VkPhysicalDevice _physicalDevice, ShaderDescriptor::Output _description)
	{
		VulkanAttachment attachment;
		attachment.image_ = VK_NULL_HANDLE;
		attachment.memory_ = VK_NULL_HANDLE;

		VkImageCreateInfo imageCreateInfo{};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = VulkanTypeConverter::Convert(_description.format_);
		imageCreateInfo.extent.width = _description.width_;
		imageCreateInfo.extent.height = _description.height_;
		imageCreateInfo.extent.depth = 1;
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.usage = VulkanTypeConverter::Convert(_description.usage_);
		vkCreateImage(logicalDevice_, &imageCreateInfo, nullptr, &*attachment.image_) >> VulkanResultChecker::Get();

		VkMemoryRequirements memoryRequirements{};
		vkGetImageMemoryRequirements(logicalDevice_, *attachment.image_, &memoryRequirements);
		VkMemoryAllocateInfo memoryAllocateInfo{};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = FindMemoryTypeIndex(_physicalDevice, memoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		vkAllocateMemory(logicalDevice_, &memoryAllocateInfo, nullptr, &*attachment.memory_);
		vkBindImageMemory(logicalDevice_, *attachment.image_, *attachment.memory_, 0);

		VkImageViewCreateInfo imageViewCreateInfo{};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = VulkanTypeConverter::Convert(_description.format_);
		imageViewCreateInfo.subresourceRange.aspectMask = GetAspectMask(imageCreateInfo.format, imageCreateInfo.usage);
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;
		imageViewCreateInfo.image = *attachment.image_;
		vkCreateImageView(logicalDevice_, &imageViewCreateInfo, nullptr, &attachment.imageView_) >> VulkanResultChecker::Get();

		attachments_.push_back(attachment);
	}

	uint32_t VulkanRenderTarget::GetNumAttachments() const
	{
		return (uint32_t)attachments_.size();
	}

	std::vector<VkImageView> VulkanRenderTarget::GetImageViews() const
	{
		std::vector<VkImageView> imageViews;
		for (const VulkanAttachment& attachment : attachments_)
		{
			imageViews.push_back(attachment.imageView_);
		}

		return imageViews;
	}

	VkFramebuffer VulkanRenderTarget::GetFramebuffer(VkRenderPass _renderPass)
	{
		auto found = framebuffers_.find(_renderPass);
		if (found != framebuffers_.end())
		{
			return found->second;
		}

		std::vector<VkImageView> imageViews;
		for (VulkanAttachment& attachment : attachments_)
		{
			imageViews.push_back(attachment.imageView_);
		}

		VkFramebuffer framebuffer = VK_NULL_HANDLE;
		VkFramebufferCreateInfo framebufferCreateInfo{};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = _renderPass;
		framebufferCreateInfo.attachmentCount = (uint32_t)imageViews.size();
		framebufferCreateInfo.width = width_;
		framebufferCreateInfo.height = height_;
		framebufferCreateInfo.layers = 1;
		framebufferCreateInfo.pAttachments = imageViews.data();
		vkCreateFramebuffer(logicalDevice_, &framebufferCreateInfo, nullptr, &framebuffer) >> VulkanResultChecker::Get();

		framebuffers_.insert(std::make_pair(_renderPass, framebuffer));
		return framebuffer;
	}

	VkImageAspectFlags VulkanRenderTarget::GetAspectMask(VkFormat _format, VkImageUsageFlags _usage)
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
}