#include "vulkan_render_target.h"
#include "vulkan_validation.hpp"
#include "vulkan_utility.h"

namespace io::graphics
{
	VulkanRenderTarget::VulkanRenderTarget(VkDevice _logicalDevice, const RenderTarget::Layout& _renderTargetLayout)
		: logicalDevice_(_logicalDevice)
		, width_(_renderTargetLayout.width_)
		, height_(_renderTargetLayout.height_)
	{
		for (RenderTarget::AttachmentDescription attachmentDescription : _renderTargetLayout.attachments_)
		{
			AddAttachment(attachmentDescription);
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
		if (framebuffer_)
		{
			vkDestroyFramebuffer(logicalDevice_, framebuffer_, nullptr);
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

	void VulkanRenderTarget::AddAttachment(RenderTarget::AttachmentDescription _description)
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
		imageCreateInfo.usage = VulkanTypeConverter::Convert(_description.usage_) | VK_IMAGE_USAGE_SAMPLED_BIT;
		vkCreateImage(logicalDevice_, &imageCreateInfo, nullptr, &*attachment.image_) >> VulkanResultChecker::GetInstance();

		VkMemoryRequirements memoryRequirements{};
		vkGetImageMemoryRequirements(logicalDevice_, *attachment.image_, &memoryRequirements);
		VkMemoryAllocateInfo memoryAllocateInfo{};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		//memoryAllocateInfo.memoryTypeIndex = memoryRequirements.memoryTypeBits;
		vkAllocateMemory(logicalDevice_, &memoryAllocateInfo, nullptr, &*attachment.memory_);
		vkBindImageMemory(logicalDevice_, *attachment.image_, *attachment.memory_, 0);

		VkImageViewCreateInfo imageViewCreateInfo{};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = VulkanTypeConverter::Convert(_description.format_);
		imageViewCreateInfo.subresourceRange.aspectMask = VulkanTypeConverter::GetAspectMask(imageCreateInfo.format, imageCreateInfo.usage);
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;
		imageViewCreateInfo.image = *attachment.image_;
		vkCreateImageView(logicalDevice_, &imageViewCreateInfo, nullptr, &attachment.imageView_) >> VulkanResultChecker::GetInstance();

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

	void VulkanRenderTarget::Bind(VkRenderPass _renderPass)
	{
		if (framebuffer_)
		{
			vkDestroyFramebuffer(logicalDevice_, framebuffer_, nullptr);
		}

		std::vector<VkImageView> imageViews;
		for (VulkanAttachment& attachment : attachments_)
		{
			imageViews.push_back(attachment.imageView_);
		}

		VkFramebufferCreateInfo framebufferCreateInfo{};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = _renderPass;
		framebufferCreateInfo.attachmentCount = (uint32_t)imageViews.size();
		framebufferCreateInfo.width = width_;
		framebufferCreateInfo.height = height_;
		framebufferCreateInfo.layers = 1;
		framebufferCreateInfo.pAttachments = imageViews.data();
		vkCreateFramebuffer(logicalDevice_, &framebufferCreateInfo, nullptr, &framebuffer_) >> VulkanResultChecker::GetInstance();
	}

	VkFramebuffer VulkanRenderTarget::GetFramebuffer() const
	{
		return framebuffer_;
	}
}