#include "vulkan_api.h"
#include "vulkan_bootstrap.h"
#include "vulkan_pipeline.h"
#include "vulkan_render_pass.h"

namespace io::graphics
{
	VulkanAPI::VulkanAPI(GLFWwindow& _window)
	{
		instance_ = VulkanBootstrap::CreateVulkanInstance();
		surface_ = VulkanBootstrap::CreateSurface(instance_, &_window);
		physicalDevice_ = VulkanBootstrap::SelectPhysicalDevice(instance_);
		logicalDevice_ = VulkanBootstrap::CreateLogicalDevice(physicalDevice_, surface_);
		graphicsQueueFamilyIndex_ = VulkanBootstrap::FindGraphicsQueueFamilyIndex(physicalDevice_, surface_);
		vkGetDeviceQueue(logicalDevice_, graphicsQueueFamilyIndex_, 0, &graphicsQueue_);
		presentQueueFamilyIndex_ = VulkanBootstrap::FindPresentQueueFamilyIndex(physicalDevice_, surface_);
		vkGetDeviceQueue(logicalDevice_, presentQueueFamilyIndex_, 0, &presentQueue_);
		commandPool_ = VulkanBootstrap::CreateCommandPool(logicalDevice_, graphicsQueueFamilyIndex_);
		commandBuffers_ = VulkanBootstrap::CreateCommandBuffers(logicalDevice_, commandPool_, frameConcurrency_);
		swapChain_ = VulkanBootstrap::CreateSwapchain(logicalDevice_, physicalDevice_, surface_);
		swapChainFormat_ = VulkanBootstrap::FindBestSurfaceFormat(physicalDevice_, surface_);
		swapChainImageViews_ = VulkanBootstrap::CreateSwapchainImageViews(logicalDevice_, swapChain_, swapChainFormat_);
		swapChainExtent_ = VulkanBootstrap::GetSurfaceCapabilities(physicalDevice_, surface_).currentExtent;
		swapChainRenderPass_ = VulkanBootstrap::CreateRenderPass(logicalDevice_, swapChainFormat_);
		swapChainFramebuffers_ = VulkanBootstrap::CreateFramebuffers(logicalDevice_, swapChainImageViews_, swapChainRenderPass_, swapChainExtent_.width, swapChainExtent_.height);
		imageAvailableSemaphores_ = VulkanBootstrap::CreateSemaphores(logicalDevice_, frameConcurrency_);
		renderFinishedSemaphores_ = VulkanBootstrap::CreateSemaphores(logicalDevice_, frameConcurrency_);
		fences_ = VulkanBootstrap::CreateFences(logicalDevice_, frameConcurrency_);

		swapChainPipeline_ = std::make_unique<VulkanPipeline>(logicalDevice_, physicalDevice_, commandPool_, graphicsQueue_, VulkanBootstrap::CreatePresentationPipelineState(), swapChainRenderPass_, 1);
	}

	VulkanAPI::~VulkanAPI()
	{
		vkDestroySemaphore(logicalDevice_, lastSemaphore_, nullptr);

		pipeline_->~VulkanPipeline();
		pipeline_.reset();
		swapChainPipeline_->~VulkanPipeline();
		swapChainPipeline_.reset();
		renderPass_->~VulkanRenderPass();
		renderPass_.reset();

		for (auto& fence : fences_)
		{
			vkDestroyFence(logicalDevice_, fence, nullptr);
		}
		for (auto& semaphore : renderFinishedSemaphores_)
		{
			vkDestroySemaphore(logicalDevice_, semaphore, nullptr);
		}
		for (auto& semaphore : imageAvailableSemaphores_)
		{
			vkDestroySemaphore(logicalDevice_, semaphore, nullptr);
		}
		for (auto& swapChainFramebuffer : swapChainFramebuffers_)
		{
			vkDestroyFramebuffer(logicalDevice_, swapChainFramebuffer, nullptr);
		}
		vkDestroyRenderPass(logicalDevice_, swapChainRenderPass_, nullptr);
		for (auto& swapChainImageView : swapChainImageViews_)
		{
			vkDestroyImageView(logicalDevice_, swapChainImageView, nullptr);
		}
		vkDestroySwapchainKHR(logicalDevice_, swapChain_, nullptr);
		vkFreeCommandBuffers(logicalDevice_, commandPool_, uint32_t(commandBuffers_.size()), commandBuffers_.data());
		vkDestroyCommandPool(logicalDevice_, commandPool_, nullptr);
		vkDestroyDevice(logicalDevice_, nullptr);
		vkDestroySurfaceKHR(instance_, surface_, nullptr);
		vkDestroyInstance(instance_, nullptr);
	}

	std::shared_ptr<RenderPass> VulkanAPI::CreateRenderPass(const RenderPassLayout& _renderPassLayout, uint32_t _width, uint32_t _height)
	{
		if (_renderPassLayout.subpassDescriptions_.empty())
		{
			throw std::exception("subpass cannot be empty");
		}

		if (_renderPassLayout.subpassDescriptions_[0].numColorReferences_ > _renderPassLayout.attachmentDescriptions_.size())
		{
			throw std::exception("subpass is referencing invalid attachment");
		}

		return std::make_shared<VulkanRenderPass>(logicalDevice_, commandPool_, _renderPassLayout, _width, _height);
	}

	std::shared_ptr<Pipeline> VulkanAPI::CreatePipeline(const PipelineState& _pipelineState, const RenderPass& _renderPass)
	{
		const auto& vulkanRenderPass = static_cast<const VulkanRenderPass&>(_renderPass);
		// have to swap graphics queue to copy queue
		return std::make_shared<VulkanPipeline>(logicalDevice_, physicalDevice_, commandPool_, graphicsQueue_, _pipelineState, vulkanRenderPass.GetInstance(), vulkanRenderPass.GetNumColorAttachements());
	}

	void VulkanAPI::BindRenderPass(std::shared_ptr<RenderPass> _renderPass)
	{
		renderPass_ = std::static_pointer_cast<VulkanRenderPass>(_renderPass);
	}

	void VulkanAPI::BindPipeline(std::shared_ptr<Pipeline> _pipeline)
	{
		pipeline_ = std::static_pointer_cast<VulkanPipeline>(_pipeline);
	}

	void VulkanAPI::BeginFrame()
	{
		vkWaitForFences(logicalDevice_, 1, &fences_[currentFrame_], VK_TRUE, UINT64_MAX);
		if (vkAcquireNextImageKHR(logicalDevice_, swapChain_, UINT64_MAX, imageAvailableSemaphores_[currentFrame_], VK_NULL_HANDLE, &currentImageIndex_) != VK_SUCCESS)
		{
			throw std::exception("failed to get avilable swap chain index");
		}

		currentFrame_ = currentImageIndex_;

		vkResetFences(logicalDevice_, 1, &fences_[currentFrame_]);
		vkResetCommandBuffer(commandBuffers_[currentFrame_], 0);

		std::erase_if(renderPassSemaphores_,
			[&](std::pair<uint32_t, VkSemaphore> _pair)
			{
				if (_pair.first == (currentFrame_ + 1) % frameConcurrency_)
				{
					vkDestroySemaphore(logicalDevice_, _pair.second, nullptr);
					return true;
				}

				return false;
			});

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		if (vkBeginCommandBuffer(commandBuffers_[currentFrame_], &beginInfo) != VK_SUCCESS)
		{
			throw std::exception("failed to begin recording command buffer");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = swapChainRenderPass_;
		renderPassInfo.framebuffer = swapChainFramebuffers_[currentImageIndex_];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = { swapChainExtent_.width, swapChainExtent_.height };
		VkClearValue clearColor{ 0.0f, 0.0f, 0.0f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffers_[currentFrame_], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		waitingSemaphore_ = imageAvailableSemaphores_[currentFrame_];
		lastSemaphore_ = VulkanBootstrap::CreateSemaphores(logicalDevice_, 1).front();
	}

	void VulkanAPI::Draw()
	{
		VkFence fence = renderPass_->GetFence();
		vkWaitForFences(logicalDevice_, 1, &fence, VK_TRUE, UINT64_MAX);
		vkResetFences(logicalDevice_, 1, &fence);
		vkResetCommandBuffer(renderPass_->GetCommandBuffer(), 0);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		if (vkBeginCommandBuffer(renderPass_->GetCommandBuffer(), &beginInfo) != VK_SUCCESS)
		{
			throw std::exception("failed to begin recording command buffer");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass_->GetInstance();
		renderPassInfo.framebuffer = renderPass_->GetFramebuffer();
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = { renderPass_->GetFramebufferWidth(), renderPass_->GetFramebufferHeight() };
		std::vector<VkClearValue> clearValues(renderPass_->GetNumFramebufferAttachments());
		for (auto& clearValue : clearValues)
		{
			clearValue.color = VkClearColorValue{};
			clearValue.depthStencil = { 1.0f, 0 };
		}
		renderPassInfo.clearValueCount = uint32_t(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(renderPass_->GetCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(renderPass_->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_->GetInstance());

		VkBuffer vertexBuffers[] = { swapChainPipeline_->GetVertexBuffer() };
		//VkBuffer indexBuffers[] = { _pipeline.GetIndexBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(renderPass_->GetCommandBuffer(), 0, 1, vertexBuffers, offsets);
		//vkCmdBindIndexBuffer(renderPass_->GetCommandBuffer(), 0, 1, indexBuffers, offsets, VK_INDEX_TYPE_UINT32);

		vkCmdDraw(renderPass_->GetCommandBuffer(), 3, 1, 0, 0);
		vkCmdEndRenderPass(renderPass_->GetCommandBuffer());

		if (vkEndCommandBuffer(renderPass_->GetCommandBuffer()) != VK_SUCCESS)
		{
			throw std::exception("failed to end command buffer");
		}

		VkSubmitInfo submitInfo{};
		VkSemaphore semaphore = renderPass_->GetSemaphore();
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &waitingSemaphore_;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		VkCommandBuffer commandBuffer = renderPass_->GetCommandBuffer();
		submitInfo.pCommandBuffers = &commandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &lastSemaphore_;

		if (vkQueueSubmit(graphicsQueue_, 1, &submitInfo, fence) != VK_SUCCESS)
		{
			throw std::exception("failed to submit draw command buffer");
		}

		waitingSemaphore_ = lastSemaphore_;
		renderPassSemaphores_.push_back({ currentFrame_, lastSemaphore_ });
	}

	void VulkanAPI::EndFrame()
	{
		vkCmdBindPipeline(commandBuffers_[currentFrame_], VK_PIPELINE_BIND_POINT_GRAPHICS, swapChainPipeline_->GetInstance());

		VkBuffer vertexBuffers[] = { swapChainPipeline_->GetVertexBuffer() };
		//VkBuffer indexBuffers[] = { _pipeline.GetIndexBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffers_[currentFrame_], 0, 1, vertexBuffers, offsets);
		//vkCmdBindIndexBuffer(commandBuffers_[currentFrame_], 0, 1, indexBuffers, offsets, VK_INDEX_TYPE_UINT32);

		vkCmdDraw(commandBuffers_[currentFrame_], 3, 1, 0, 0);
		vkCmdEndRenderPass(commandBuffers_[currentFrame_]);

		if (vkEndCommandBuffer(commandBuffers_[currentFrame_]) != VK_SUCCESS)
		{
			throw std::exception("failed to end command buffer");
		}

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		//VkSemaphore waitSemaphores[] = { imageAvailableSemaphores_[currentFrame_] };
		VkSemaphore waitSemaphores[] = { lastSemaphore_ };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers_[currentFrame_];
		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores_[currentFrame_] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(graphicsQueue_, 1, &submitInfo, fences_[currentFrame_]) != VK_SUCCESS)
		{
			throw std::exception("failed to submit draw command buffer");
		}

		VkSwapchainKHR swapChains[] = { swapChain_ };
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &currentImageIndex_;
		presentInfo.pResults = nullptr; // Optional

		vkQueuePresentKHR(presentQueue_, &presentInfo);

		currentFrame_ = (currentFrame_ + 1) % frameConcurrency_;
	}
}
