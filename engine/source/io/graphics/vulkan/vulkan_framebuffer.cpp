#include "vulkan_framebuffer.h"
#include "vulkan_validation.hpp"
#include "vulkan_utility.h"

namespace io::graphics
{
	VulkanFramebuffer::VulkanFramebuffer(VkDevice _logicalDevice, VkRenderPass _renderPass, const Framebuffer::Layout& _framebufferLayout)
		: logicalDevice_(_logicalDevice)
		, width_(_framebufferLayout.width_)
		, height_(_framebufferLayout.height_)
	{
		InitializeAttachments(_logicalDevice, _framebufferLayout);
		CreateInstance(_logicalDevice, _renderPass);
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		for (auto& attachment : attachments_)
		{
			vkDestroyImageView(logicalDevice_, attachment.imageView_, nullptr);

			if (attachment.image_ == VK_NULL_HANDLE)
			{
				continue;
			}

			vkFreeMemory(logicalDevice_, attachment.memory_, nullptr);
			vkDestroyImage(logicalDevice_, attachment.image_, nullptr);
		}

		vkDestroyFramebuffer(logicalDevice_, instance_, nullptr);
	}

	VkFramebuffer VulkanFramebuffer::GetInstance() const
	{
		return instance_;
	}

	uint32_t VulkanFramebuffer::GetNumAttachements(bool _excludeDepthAttachement) const
	{
		return (uint32_t)attachments_.size();
	}

	void VulkanFramebuffer::InitializeAttachments(VkDevice _logicalDevice,  const Framebuffer::Layout& _framebufferLayout)
	{
		for (const auto& attachementDescription : _framebufferLayout.attachments_)
		{
			VulkanAttachment attachment;
			if (auto imageView = static_cast<VkImageView>(attachementDescription.attachmentHandle_))
			{
				attachment.imageView_ = imageView;
			}
			else
			{
				VkImageCreateInfo imageCreateInfo{};
				imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
				imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
				imageCreateInfo.format = VulkanTypeConverter::Convert(attachementDescription.format_);
				imageCreateInfo.extent.width = attachementDescription.width_;
				imageCreateInfo.extent.height = attachementDescription.height_;
				imageCreateInfo.extent.depth = 1;
				imageCreateInfo.mipLevels = 1;
				imageCreateInfo.arrayLayers = 1;
				imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
				imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
				imageCreateInfo.usage = VulkanTypeConverter::Convert(attachementDescription.usage_) | VK_IMAGE_USAGE_SAMPLED_BIT;
				vkCreateImage(_logicalDevice, &imageCreateInfo, nullptr, &attachment.image_) >> VulkanResultChecker::GetInstance();

				VkMemoryRequirements memoryRequirements{};
				vkGetImageMemoryRequirements(_logicalDevice, attachment.image_, &memoryRequirements);
				VkMemoryAllocateInfo memoryAllocateInfo{};
				memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				memoryAllocateInfo.allocationSize = memoryRequirements.size;
				//memoryAllocateInfo.memoryTypeIndex = memoryRequirements.memoryTypeBits;
				vkAllocateMemory(_logicalDevice, &memoryAllocateInfo, nullptr, &attachment.memory_);
				vkBindImageMemory(_logicalDevice, attachment.image_, attachment.memory_, 0);

				VkImageViewCreateInfo imageViewCreateInfo{};
				imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				imageViewCreateInfo.format = VulkanTypeConverter::Convert(attachementDescription.format_);
				imageViewCreateInfo.subresourceRange.aspectMask = VulkanTypeConverter::GetAspectMask(imageCreateInfo.format, imageCreateInfo.usage);
				imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
				imageViewCreateInfo.subresourceRange.levelCount = 1;
				imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
				imageViewCreateInfo.subresourceRange.layerCount = 1;
				imageViewCreateInfo.image = attachment.image_;
				vkCreateImageView(_logicalDevice, &imageViewCreateInfo, nullptr, &attachment.imageView_) >> VulkanResultChecker::GetInstance();
			}
			attachments_.push_back(attachment);
		}
	}

	void VulkanFramebuffer::CreateInstance(VkDevice _logicalDevice, VkRenderPass _renderPass)
	{
		std::vector<VkImageView> imageViews;
		for (VulkanAttachment& attachment : attachments_)
		{
			imageViews.push_back(attachment.imageView_);
		}

		VkFramebufferCreateInfo framebufferCreateInfo{};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = _renderPass;
		framebufferCreateInfo.attachmentCount = (uint32_t)attachments_.size();
		framebufferCreateInfo.width = width_;
		framebufferCreateInfo.height = height_;
		framebufferCreateInfo.layers = 1;
		framebufferCreateInfo.pAttachments = imageViews.data();
		vkCreateFramebuffer(_logicalDevice, &framebufferCreateInfo, nullptr, &instance_) >> VulkanResultChecker::GetInstance();
	}
}