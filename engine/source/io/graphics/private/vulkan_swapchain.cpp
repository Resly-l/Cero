#include "vulkan_swapchain.h"
#include "vulkan_device.h"

namespace io::graphics
{
	void VulkanSwapChain::Initialize(const VulkanDevice& _device, VkSurfaceKHR _surface, uint32_t _width, uint32_t _height)
	{
		width_ = _width;
		height_ = _height;

		CreateSwapChain(_device, _surface);
		CreateRenderPass(_device);
		InitializeImages(_device);
		CreateFrameBuffer(_device);
	}

	void VulkanSwapChain::Release(const VulkanDevice& _device)
	{
		for (auto framebuffer : framebuffers_)
		{
			vkDestroyFramebuffer(_device.GetLogicalDevice(), framebuffer, nullptr);
		}

		vkDestroyRenderPass(_device.GetLogicalDevice(), renderPass_, nullptr);

		for (auto imageView : imageViews_)
		{
			vkDestroyImageView(_device.GetLogicalDevice(), imageView, nullptr);
		}

		vkDestroySwapchainKHR(_device.GetLogicalDevice(), instance_, nullptr);
	}

    VkSwapchainKHR VulkanSwapChain::GetInstance() const
    {
		return instance_;
    }

	const std::vector<VkFramebuffer>& VulkanSwapChain::GetFrameBuffers() const
	{
		return framebuffers_;
	}

	VkRenderPass VulkanSwapChain::GetRenderPass() const
	{
		return renderPass_;
	}

	uint32_t VulkanSwapChain::GetWidth() const
	{
		return width_;
	}

	uint32_t VulkanSwapChain::GetHeight() const
	{
		return height_;
	}

	void VulkanSwapChain::CreateSwapChain(const VulkanDevice& _device, VkSurfaceKHR _surface)
	{
		VkSurfaceCapabilitiesKHR capabilities{};
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_device.GetPhysicalDevice(), _surface, &capabilities);

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = _surface;
		createInfo.minImageCount = capabilities.minImageCount + 1;
		createInfo.imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
		createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		createInfo.imageExtent = { width_, height_ };
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		uint32_t queueFamilyIndices[] = { _device.GetGraphicsQueueFamilyIndex(), _device.GetPresentQueueFamilyIndex() };

		if (queueFamilyIndices[0] != queueFamilyIndices[1])
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		createInfo.preTransform = capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
		createInfo.clipped = VK_TRUE;

		if (vkCreateSwapchainKHR(_device.GetLogicalDevice(), &createInfo, nullptr, &instance_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create swap chain");
		}
	}

	void VulkanSwapChain::InitializeImages(const VulkanDevice& _device)
	{
		uint32_t imageCount = 0;
		vkGetSwapchainImagesKHR(_device.GetLogicalDevice(), instance_, &imageCount, nullptr);
		images_.resize(imageCount);
		if (vkGetSwapchainImagesKHR(_device.GetLogicalDevice(), instance_, &imageCount, images_.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to get swap chain images");
		}

		imageViews_.resize(images_.size());

		for (size_t i = 0; i < images_.size(); i++)
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = images_[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = VK_FORMAT_B8G8R8A8_SRGB;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(_device.GetLogicalDevice(), &createInfo, nullptr, &imageViews_[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create image views");
			}
		}
	}

	void VulkanSwapChain::CreateRenderPass(const VulkanDevice& _device)
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = VK_FORMAT_B8G8R8A8_SRGB;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(_device.GetLogicalDevice(), &renderPassInfo, nullptr, &renderPass_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create render pass");
		}
	}

	void VulkanSwapChain::CreateFrameBuffer(const VulkanDevice& _device)
	{
		framebuffers_.resize(imageViews_.size());

		for (size_t i = 0; i < imageViews_.size(); i++)
		{
			VkImageView attachments[] =
			{
				imageViews_[i]
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass_;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.flags;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = width_;
			framebufferInfo.height = height_;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(_device.GetLogicalDevice(), &framebufferInfo, nullptr, &framebuffers_[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create framebuffer");
			}
		}
	}
}