#include "vulkan_api.h"
#include "vulkan_pipeline.h"
#include "vulkan_validation.hpp"
#include "io/window/window_min.h"
#include <vulkan/vulkan_win32.h>
#include "thirdparty/glfw/glfw3.h"

namespace io::graphics
{
	VulkanAPI::VulkanAPI(void* _window)
	{
		CreateInstance();
		SelectPhysicalDevice(_window);
		CreateLogicalDevice();
		CreateSwapchain();
		CreatePresentationPipeline();
		CreateCommandPool();
		CreateCommandBuffers();
		CreateSyncObjects();
	}

	VulkanAPI::~VulkanAPI()
	{
		vkDeviceWaitIdle(logicalDevice_);

		for (Frame& frame : frames_)
		{
			vkDestroySemaphore(logicalDevice_, frame.imageAcquiringSemaphore_, nullptr);
			vkDestroySemaphore(logicalDevice_, frame.commandExecutionSemaphore_, nullptr);
			vkDestroyFence(logicalDevice_, frame.frameFence_, nullptr);
			vkFreeCommandBuffers(logicalDevice_, commandPool_, 1, &frame.commandBuffer_);
		}

		vkDestroyCommandPool(logicalDevice_, commandPool_, nullptr);
		presentationPipeline_ = nullptr;

		vkb::destroy_swapchain(swapchain_);
		vkb::destroy_surface(instance_, logicalDevice_.surface);
		vkb::destroy_device(logicalDevice_);
		vkb::destroy_instance(instance_);
	}

	std::shared_ptr<Pipeline> VulkanAPI::CreatePipeline(const Pipeline::Layout& _pipelineLayout)
	{
		return nullptr;
	}

	std::shared_ptr<RenderTarget> VulkanAPI::CreateRenderTarget(const RenderTargetLayout& _renderTargetLayout)
	{
		return nullptr;
	}

	void VulkanAPI::BindPipeline(std::shared_ptr<Pipeline> _pipeline)
	{

	}

	void VulkanAPI::BindRenderTargets(std::vector<std::shared_ptr<RenderTarget>> _renderTargets)
	{

	}

	void VulkanAPI::BeginFrame()
	{

	}

	void VulkanAPI::Draw()
	{
	}

	void VulkanAPI::EndFrame()
	{
		vkWaitForFences(logicalDevice_, 1, &frames_[frameIndex_].frameFence_, VK_TRUE, UINT64_MAX);
		vkResetFences(logicalDevice_, 1, &frames_[frameIndex_].frameFence_);
		vkAcquireNextImageKHR(logicalDevice_, swapchain_, UINT64_MAX, frames_[frameIndex_].imageAcquiringSemaphore_, VK_NULL_HANDLE, &swapChainImageIndex_);

		VkCommandBufferBeginInfo commandBufferBeginInfo{};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = 0;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;
		vkResetCommandBuffer(frames_[frameIndex_].commandBuffer_, VkCommandBufferResetFlags{});
		vkBeginCommandBuffer(frames_[frameIndex_].commandBuffer_, &commandBufferBeginInfo);

		VkRenderPassBeginInfo renderPassBeginInfo{};
		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = presentationPipeline_->GetRenderPass();
		renderPassBeginInfo.framebuffer = presentationPipeline_->GetFramebuffer(swapChainImageIndex_);
		renderPassBeginInfo.renderArea.offset = {0, 0};
		renderPassBeginInfo.renderArea.extent = swapchain_.extent;
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearColor;
		vkCmdBeginRenderPass(frames_[frameIndex_].commandBuffer_, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(frames_[frameIndex_].commandBuffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, presentationPipeline_->GetInstance());

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapchain_.extent.width);
		viewport.height = static_cast<float>(swapchain_.extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(frames_[frameIndex_].commandBuffer_, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = swapchain_.extent;
		vkCmdSetScissor(frames_[frameIndex_].commandBuffer_, 0, 1, &scissor);

		vkCmdDraw(frames_[frameIndex_].commandBuffer_, 3, 1, 0, 0);
		vkCmdEndRenderPass(frames_[frameIndex_].commandBuffer_);
		vkEndCommandBuffer(frames_[frameIndex_].commandBuffer_) >> VulkanResultChecker::GetInstance();

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		VkSemaphore waitSemaphores[] = { frames_[frameIndex_].imageAcquiringSemaphore_ };
		VkSemaphore signalSemaphores[] = { frames_[frameIndex_].commandExecutionSemaphore_ };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &frames_[frameIndex_].commandBuffer_;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;
		vkQueueSubmit(logicalDevice_.get_queue(vkb::QueueType::graphics).value(), 1, &submitInfo, frames_[frameIndex_].frameFence_) >> VulkanResultChecker::GetInstance();

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapchains[] = { swapchain_ };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapchains;
		presentInfo.pImageIndices = &swapChainImageIndex_;

		vkQueuePresentKHR(logicalDevice_.get_queue(vkb::QueueType::present).value(), &presentInfo);
		frameIndex_ = (frameIndex_ + 1) % numFrameConcurrency_;
	}

	void VulkanAPI::CreateInstance()
	{
		vkb::InstanceBuilder builder;
		builder.request_validation_layers();
		builder.use_default_debug_messenger();
		instance_ = Build(builder, &vkb::InstanceBuilder::build);
	}

	void VulkanAPI::SelectPhysicalDevice(void* _window)
	{
		VkSurfaceKHR surface;
		VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
		HINSTANCE instance = GetModuleHandle(nullptr);
		surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceCreateInfo.hinstance = instance;
		surfaceCreateInfo.hwnd = (HWND)_window;
		vkCreateWin32SurfaceKHR(instance_, &surfaceCreateInfo, nullptr, &surface) >> VulkanResultChecker::GetInstance();

		vkb::PhysicalDeviceSelector deviceSelector(instance_);
		deviceSelector.set_surface(surface);
		physicalDevice_ = Build(deviceSelector, &vkb::PhysicalDeviceSelector::select, vkb::DeviceSelectionMode::partially_and_fully_suitable);
	}

	void VulkanAPI::CreateLogicalDevice()
	{
		vkb::DeviceBuilder deviceBuilder(physicalDevice_);
		logicalDevice_ = Build(deviceBuilder, &vkb::DeviceBuilder::build);
	}

	void VulkanAPI::CreateSwapchain()
	{
		vkb::SwapchainBuilder swapchainBuilder(logicalDevice_);
		VkSurfaceFormatKHR fallbackFormat{};
		fallbackFormat.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		fallbackFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
		swapchainBuilder.add_fallback_format(fallbackFormat);
		swapchainBuilder.use_default_image_usage_flags();
		swapchainBuilder.use_default_present_mode_selection();
		swapchain_ = Build(swapchainBuilder, &vkb::SwapchainBuilder::build);
	}

	void VulkanAPI::CreatePresentationPipeline()
	{
		Pipeline::Layout pipelineLayout{};
		pipelineLayout.vertexShaderPath = L"../../../engine/asset/shader/bin/hello_triangle.vert.spv";
		pipelineLayout.pixelShaderPath = L"../../../engine/asset/shader/bin/hello_triangle.frag.spv";

		Framebuffer::Layout framebufferLayout;
		framebufferLayout.width_ = swapchain_.extent.width;
		framebufferLayout.height_ = swapchain_.extent.height;

		Framebuffer::AttachmentDescription attachmentDescription;
		attachmentDescription.format_ = ImageFormat::B8G8R8A8_UNORM;
		attachmentDescription.usage_ = ImageUsage::COLOR_ATTACHMENT;
		attachmentDescription.width_ = swapchain_.extent.width;
		attachmentDescription.height_ = swapchain_.extent.height;

		for (VkImageView& imageView : swapchain_.get_image_views().value())
		{
			framebufferLayout.attachments_.clear();
			attachmentDescription.attachmentHandle_ = imageView;
			framebufferLayout.attachments_.push_back(attachmentDescription);
			pipelineLayout.frameBufferLayouts.push_back(framebufferLayout);
		}

		presentationPipeline_ = std::make_unique<VulkanPipeline>(logicalDevice_, physicalDevice_, pipelineLayout);
	}

	void VulkanAPI::CreateCommandPool()
	{
		VkCommandPoolCreateInfo commandPoolCreateInfo{};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = logicalDevice_.get_queue_index(vkb::QueueType::graphics).value();
		vkCreateCommandPool(logicalDevice_, &commandPoolCreateInfo, nullptr, &commandPool_) >> VulkanResultChecker::GetInstance();
	}

	void VulkanAPI::CreateCommandBuffers()
	{
		for (Frame& frame : frames_)
		{
			VkCommandBufferAllocateInfo allocateInfo{};
			allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocateInfo.commandPool = commandPool_;
			allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocateInfo.commandBufferCount = 1;
			vkAllocateCommandBuffers(logicalDevice_, &allocateInfo, &frame.commandBuffer_) >> VulkanResultChecker::GetInstance();
		}
	}

	void VulkanAPI::CreateSyncObjects()
	{
		for (Frame& frame : frames_)
		{
			VkSemaphoreCreateInfo semaphoreCreateInfo{};
			semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			vkCreateSemaphore(logicalDevice_, &semaphoreCreateInfo, nullptr, &frame.imageAcquiringSemaphore_);
			vkCreateSemaphore(logicalDevice_, &semaphoreCreateInfo, nullptr, &frame.commandExecutionSemaphore_);

			VkFenceCreateInfo fenceCreateInfo{};
			fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			vkCreateFence(logicalDevice_, &fenceCreateInfo, nullptr, &frame.frameFence_);
		}
	}
}
