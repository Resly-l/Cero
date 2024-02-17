#include "vulkan_render_pass.h"
#include "vulkan_bootstrap.h"
#include "vulkan_attachment.h"

namespace io::graphics
{
	VulkanRenderPass::VulkanRenderPass(const VkDevice& _logicalDevice, VkCommandPool _commandPool, const RenderPassLayout& _renderPassLayout, uint32_t _width, uint32_t _height)
		: logicalDevice_(_logicalDevice)
		, framebufferWidth_(_width)
		, framebufferHeight_(_height)
	{
		commandBuffer_ = VulkanBootstrap::CreateCommandBuffers(logicalDevice_, _commandPool, 1)[0];

		std::vector<VkAttachmentDescription> attachmentDescriptions;
		for (const auto& attachmentDescription : _renderPassLayout.attachmentDescriptions_)
		{
			VkAttachmentDescription description{};
			description.format = VulkanBootstrap::ConvertFormat(attachmentDescription.format_).format;
			description.samples = VK_SAMPLE_COUNT_1_BIT;
			description.loadOp = VulkanBootstrap::ConvertAttachmentLoadOperation(attachmentDescription.loadOperation_);
			description.storeOp = VulkanBootstrap::ConvertAttachmentStoreOperation(attachmentDescription.storeOperation_);
			description.stencilLoadOp = VulkanBootstrap::ConvertAttachmentLoadOperation(attachmentDescription.stencilLoadOperation_);
			description.stencilStoreOp = VulkanBootstrap::ConvertAttachmentStoreOperation(attachmentDescription.stencilStoreOperation_);
			description.initialLayout = VulkanBootstrap::ConvertImageLayout(attachmentDescription.initialLayout_);
			description.finalLayout = VulkanBootstrap::ConvertImageLayout(attachmentDescription.finalLayout_);
			attachmentDescriptions.push_back(description);

			if (attachmentDescription.usage_ == ImageUsage::iuCOLOR_ATTACHMENT)
			{
				numColorAttachments_++;
			}
		}

		std::vector<VkSubpassDescription> subpassDescriptions;
		std::vector<VkAttachmentReference> colorReferences;
		for (const auto& subpassDescription : _renderPassLayout.subpassDescriptions_)
		{
			colorReferences.resize(subpassDescription.numColorReferences_);
			for (uint32_t i = 0; i < colorReferences.size(); i++)
			{
				colorReferences[i].attachment = i;
				colorReferences[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}
			VkAttachmentReference depthStencilReference{};
			depthStencilReference.attachment = colorReferences.size();
			depthStencilReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			VkSubpassDescription description{};
			description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			description.colorAttachmentCount = subpassDescription.numColorReferences_;
			description.pColorAttachments = colorReferences.data();
			description.pDepthStencilAttachment = &depthStencilReference;
			// description.inputAttachmentCount = 0;
			// description.pInputAttachments = nullptr;
			// description.preserveAttachmentCount = 0;
			// description.pPreserveAttachments = nullptr;
			// description.pResolveAttachments = nullptr;
			subpassDescriptions.push_back(description);
		}

		std::vector<VkSubpassDependency> dependencyDescriptions;
		for (const auto& dependencyDescription : _renderPassLayout.dependencyDescriptions_)
		{
			VkSubpassDependency description{};
			if (dependencyDescription.source_)
			{
				description.srcSubpass = VK_SUBPASS_EXTERNAL;
				description.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
				description.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				description.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
				description.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				description.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
			}
			else
			{
				description.dstSubpass = VK_SUBPASS_EXTERNAL;
				description.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				description.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
				description.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				description.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
				description.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
			}

			dependencyDescriptions.push_back(description);
		}

		VkRenderPassCreateInfo renderPassCreateInfo{};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.attachmentCount = uint32_t(attachmentDescriptions.size());
		renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
		renderPassCreateInfo.subpassCount = uint32_t(subpassDescriptions.size());
		renderPassCreateInfo.pSubpasses = subpassDescriptions.data();
		renderPassCreateInfo.dependencyCount = uint32_t(dependencyDescriptions.size());
		renderPassCreateInfo.pDependencies = dependencyDescriptions.data();
		vkCreateRenderPass(logicalDevice_, &renderPassCreateInfo, nullptr, &instance_);

		for (const auto& attachementDescription : _renderPassLayout.attachmentDescriptions_)
		{
			auto format = VulkanBootstrap::ConvertFormat(attachementDescription.format_);
			auto usage = VulkanBootstrap::ConvertImageUsage(attachementDescription.usage_);
			framebufferAttachments_.push_back(std::make_unique<VulkanAttachment>(logicalDevice_, format, usage, attachementDescription.width_, attachementDescription.height_, 1));
		}

		std::vector<VkImageView> imageViews;
		for (auto& framebufferAttachment : framebufferAttachments_)
		{
			imageViews.push_back(framebufferAttachment->GetImageView());
		}

		VkFramebufferCreateInfo framebufferCreateInfo{};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.pNext = nullptr;
		framebufferCreateInfo.renderPass = instance_;
		framebufferCreateInfo.pAttachments = imageViews.data();
		framebufferCreateInfo.attachmentCount = uint32_t(imageViews.size());
		framebufferCreateInfo.width = _width;
		framebufferCreateInfo.height = _height;
		framebufferCreateInfo.layers = 1;
		vkCreateFramebuffer(logicalDevice_, &framebufferCreateInfo, nullptr, &framebuffer_);

		semaphore_ = VulkanBootstrap::CreateSemaphores(logicalDevice_, 1).front();
		fence_ = VulkanBootstrap::CreateFences(logicalDevice_, 1).front();
	}

	VulkanRenderPass::~VulkanRenderPass()
	{
		for (auto& attachment : framebufferAttachments_)
		{
			attachment->~VulkanAttachment();
		}

		vkDestroyFramebuffer(logicalDevice_, framebuffer_, nullptr);
		vkDestroyRenderPass(logicalDevice_, instance_, nullptr);
	}

	VkRenderPass VulkanRenderPass::GetInstance() const
	{
		return instance_;
	}

	VkCommandBuffer VulkanRenderPass::GetCommandBuffer() const
	{
		return commandBuffer_;
	}

	VkSemaphore VulkanRenderPass::GetSemaphore() const
	{
		return semaphore_;
	}

	VkFence VulkanRenderPass::GetFence() const
	{
		return fence_;
	}

	uint32_t VulkanRenderPass::GetNumColorAttachements() const
	{
		return numColorAttachments_;
	}

	uint32_t VulkanRenderPass::GetNumFramebufferAttachments() const
	{
		return uint32_t(framebufferAttachments_.size());
	}

	VkFramebuffer VulkanRenderPass::GetFramebuffer() const
	{
		return framebuffer_;
	}

	uint32_t VulkanRenderPass::GetFramebufferWidth() const
	{
		return framebufferWidth_;
	}

	uint32_t VulkanRenderPass::GetFramebufferHeight() const
	{
		return framebufferHeight_;
	}
}