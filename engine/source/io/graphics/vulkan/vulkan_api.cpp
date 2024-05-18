#include "vulkan_api.h"
#include "vulkan_pipeline.h"
#include "vulkan_render_target.h"
#include "vulkan_mesh.h"
#include "vulkan_validation.hpp"
#include "vulkan_utility.h"
#include "io/window/window_min.h"
#include <vulkan/vulkan_win32.h>
#include "core/math/vector.h"

namespace io::graphics
{
	VulkanAPI::VulkanAPI(void* _window)
		: windowHandle_(_window)
	{
		frames_.resize(config_.numFrameConcurrency_);

		CreateInstance();
		SelectPhysicalDevice(_window);
		CreateLogicalDevice();
		CreateSwapchain();
		CreatePresentationPipeline();
		CreateSwapchainRenderTargets();
		CreateCommandPools();
		CreateCommandBuffers();
		CreateSyncObjects();

		Mesh::Layout meshLayout;

		utility::ByteBuffer::Layout vertexBufferLayout;
		vertexBufferLayout.AddAttribute<math::Vec2<float>>("position");
		vertexBufferLayout.AddAttribute<math::Vec3<float>>("color");
		meshLayout.vertices_.SetLayout(vertexBufferLayout);
		if (auto vertex = meshLayout.vertices_.Add())
		{
			vertex->Get<math::Vec2<float>>("position") = math::Vec2(-0.35f, -0.5f);
			vertex->Get<math::Vec3<float>>("color") = math::Vec3(0.98f, 0.75f, 0.85f);
		}
		if (auto vertex = meshLayout.vertices_.Add())
		{
			vertex->Get<math::Vec2<float>>("position") = math::Vec2(0.35f, -0.5f);
			vertex->Get<math::Vec3<float>>("color") = math::Vec3(0.69f, 0.22f, 0.25f);
		}
		if (auto vertex = meshLayout.vertices_.Add())
		{
			vertex->Get<math::Vec2<float>>("position") = math::Vec2(0.35f, 0.5f);
			vertex->Get<math::Vec3<float>>("color") = math::Vec3(0.98f, 0.75f, 0.85f);
		}
		if (auto vertex = meshLayout.vertices_.Add())
		{
			vertex->Get<math::Vec2<float>>("position") = math::Vec2(-0.35f, 0.5f);
			vertex->Get<math::Vec3<float>>("color") = math::Vec3(0.15f, 0.15f, 0.15f);
		}
		meshLayout.indices_ = { 0,1,2,2,3,0 };

		testMesh_ = std::make_shared<VulkanMesh>(logicalDevice_, physicalDevice_, logicalDevice_.get_queue(vkb::QueueType::transfer).value(), transfereCommandPool_, meshLayout);

		for (auto& renderTarget : swapchainRenderTargets_)
		{
			renderTarget->Bind(presentationPipeline_->GetRenderPass());
		}
	}

	VulkanAPI::~VulkanAPI()
	{
		vkDeviceWaitIdle(logicalDevice_);

		swapchainRenderTargets_.clear();
		testMesh_ = nullptr;

		for (Frame& frame : frames_)
		{
			vkDestroySemaphore(logicalDevice_, frame.imageAcquiringSemaphore_, nullptr);
			vkDestroySemaphore(logicalDevice_, frame.commandExecutionSemaphore_, nullptr);
			vkDestroyFence(logicalDevice_, frame.frameFence_, nullptr);
			vkFreeCommandBuffers(logicalDevice_, commandPool_, 1, &frame.commandBuffer_);
		}

		vkDestroyCommandPool(logicalDevice_, transfereCommandPool_, nullptr);
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

	std::shared_ptr<RenderTarget> VulkanAPI::CreateRenderTarget(const RenderTarget::Layout& _renderTargetLayout)
	{
		return nullptr;
	}

	std::shared_ptr<Mesh> VulkanAPI::CreateMesh(const Mesh::Layout& _meshLayout)
	{
		return nullptr;
	}

	void VulkanAPI::BindPipeline(std::shared_ptr<Pipeline> _pipeline)
	{

	}

	void VulkanAPI::BindRenderTarget(std::shared_ptr<RenderTarget> _renderTarget)
	{

	}

	void VulkanAPI::BindMesh(std::shared_ptr<Mesh> _mesh)
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
		if (pendingSwapchainRecreation_ && !RecreateSwapchain())
		{
			return;
		}

		vkWaitForFences(logicalDevice_, 1, &frames_[frameIndex_].frameFence_, VK_TRUE, UINT64_MAX);
		vkResetFences(logicalDevice_, 1, &frames_[frameIndex_].frameFence_) >> VulkanResultChecker::GetInstance();

		VkResult result = vkAcquireNextImageKHR(logicalDevice_, swapchain_, UINT64_MAX, frames_[frameIndex_].imageAcquiringSemaphore_, VK_NULL_HANDLE, &swapchainImageIndex_);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapchain();
			return;
		}

		VkCommandBufferBeginInfo commandBufferBeginInfo{};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		vkResetCommandBuffer(frames_[frameIndex_].commandBuffer_, VkCommandBufferResetFlags{});
		vkBeginCommandBuffer(frames_[frameIndex_].commandBuffer_, &commandBufferBeginInfo);

		VkClearValue clearValues[2];
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = presentationPipeline_->GetRenderPass();
		renderPassBeginInfo.framebuffer = swapchainRenderTargets_[swapchainImageIndex_]->GetFramebuffer();
		renderPassBeginInfo.renderArea.extent = swapchain_.extent;
		renderPassBeginInfo.clearValueCount = 2;
		renderPassBeginInfo.pClearValues = clearValues;
		vkCmdBeginRenderPass(frames_[frameIndex_].commandBuffer_, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

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

		VkBuffer vertexBuffers[] = { testMesh_->GetVertexBuffer()};
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindPipeline(frames_[frameIndex_].commandBuffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, presentationPipeline_->GetInstance());
		vkCmdBindVertexBuffers(frames_[frameIndex_].commandBuffer_, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(frames_[frameIndex_].commandBuffer_, testMesh_->GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(frames_[frameIndex_].commandBuffer_, testMesh_->GetNumIndices(), 1, 0, 0, 0);
		vkCmdEndRenderPass(frames_[frameIndex_].commandBuffer_);
		vkEndCommandBuffer(frames_[frameIndex_].commandBuffer_) >> VulkanResultChecker::GetInstance();

		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &frames_[frameIndex_].commandBuffer_;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &frames_[frameIndex_].imageAcquiringSemaphore_;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &frames_[frameIndex_].commandExecutionSemaphore_;
		vkQueueSubmit(logicalDevice_.get_queue(vkb::QueueType::graphics).value(), 1, &submitInfo, frames_[frameIndex_].frameFence_) >> VulkanResultChecker::GetInstance();

		VkSwapchainKHR swapchains[] = { swapchain_ };
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &frames_[frameIndex_].commandExecutionSemaphore_;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapchains;
		presentInfo.pImageIndices = &swapchainImageIndex_;

		result = vkQueuePresentKHR(logicalDevice_.get_queue(vkb::QueueType::present).value(), &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapchain();
		}

		frameIndex_ = (frameIndex_ + 1) % config_.numFrameConcurrency_;
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
		pipelineLayout.vertexShaderPath_ = L"../../../engine/asset/shader/bin/hello_triangle.vert.spv";
		pipelineLayout.pixelShaderPath_ = L"../../../engine/asset/shader/bin/hello_triangle.frag.spv";
		pipelineLayout.vertexInputLayout_.AddAttribute<math::Vec2<float>>("position");
		pipelineLayout.vertexInputLayout_.AddAttribute<math::Vec3<float>>("color");
		presentationPipeline_ = std::make_unique<VulkanPipeline>(logicalDevice_, physicalDevice_, pipelineLayout);
	}

	void VulkanAPI::CreateSwapchainRenderTargets()
	{
		swapchainRenderTargets_.clear();

		for (VkImageView swapchainImageView : swapchain_.get_image_views().value())
		{
			auto renderTarget = std::make_unique<VulkanRenderTarget>(logicalDevice_, swapchain_.extent.width, swapchain_.extent.height, swapchainImageView);
			RenderTarget::AttachmentDescription depthStencilDescription{};
			depthStencilDescription.width_ = swapchain_.extent.width;
			depthStencilDescription.height_ = swapchain_.extent.height;
			depthStencilDescription.format_ = ImageFormat::D32_SFLOAT_U8_UINT;
			depthStencilDescription.usage_ = ImageUsage::DEPTH_STENCIL;
			renderTarget->AddAttachment(depthStencilDescription);
			swapchainRenderTargets_.push_back(std::move(renderTarget));
		}
	}

	void VulkanAPI::CreateCommandPools()
	{
		VkCommandPoolCreateInfo commandPoolCreateInfo{};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = logicalDevice_.get_queue_index(vkb::QueueType::graphics).value();
		vkCreateCommandPool(logicalDevice_, &commandPoolCreateInfo, nullptr, &commandPool_) >> VulkanResultChecker::GetInstance();

		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		commandPoolCreateInfo.queueFamilyIndex = logicalDevice_.get_queue_index(vkb::QueueType::transfer).value();
		vkCreateCommandPool(logicalDevice_, &commandPoolCreateInfo, nullptr, &transfereCommandPool_) >> VulkanResultChecker::GetInstance();
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

	bool VulkanAPI::RecreateSwapchain()
	{
		while (IsIconic((HWND)windowHandle_))
		{
			pendingSwapchainRecreation_ = true;
			return false;
		}

		pendingSwapchainRecreation_ = false;
		vkDeviceWaitIdle(logicalDevice_);

		presentationPipeline_ = nullptr;
		vkb::destroy_swapchain(swapchain_);

		CreateSwapchain();
		CreatePresentationPipeline();
		CreateSwapchainRenderTargets();

		for (auto& renderTarget : swapchainRenderTargets_)
		{
			renderTarget->Bind(presentationPipeline_->GetRenderPass());
		}

		return true;
	}
}
