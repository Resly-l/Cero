#include "vulkan_api.h"
#include "vulkan_result.hpp"
#include "vulkan_utility.h"
#include "vulkan_pipeline.h"
#include "vulkan_render_target.h"
#include "vulkan_mesh.h"
#include "vulkan_material.h"
#include "vulkan_uniform_buffer.h"
#include "vulkan_texture.h"
#include "vulkan_render_pass.h"
#include "window/window_min.h"
#include <vulkan/vulkan_win32.h>
#include "utility/log.h"
#include "math/vector.h"
#include "math/matrix.h"
#include <iostream>

using utility::Log;

namespace graphics
{
	VulkanAPI::VulkanAPI(void* _window)
		: windowHandle_(_window)
	{
		frames_.resize(config_.numFrameConcurrency_);

		CreateInstance();
		SelectPhysicalDevice(_window);
		CreateLogicalDevice();
		CreateSwapchain();
		CreateCommandPools();
		CreateDescriptorPool();
		CreateSwapchainRenderTargets();
		CreateSyncObjects();
	}

	VulkanAPI::~VulkanAPI()
	{
		vkDeviceWaitIdle(logicalDevice_);

		swapchainRenderTargets_.clear();

		for (Frame& frame : frames_)
		{
			vkDestroySemaphore(logicalDevice_, frame.imageAcquiringSemaphore_, nullptr);
			vkDestroySemaphore(logicalDevice_, frame.commandExecutionSemaphore_, nullptr);
			vkDestroyFence(logicalDevice_, frame.frameFence_, nullptr);
			//vkFreeCommandBuffers(logicalDevice_, commandPool_, 1, &frame.commandBuffer_);
		}

		vkDestroyDescriptorPool(logicalDevice_, descriptorPool_, nullptr);
		vkDestroyCommandPool(logicalDevice_, transferCommandPool_, nullptr);
		vkDestroyCommandPool(logicalDevice_, commandPool_, nullptr);

		vkb::destroy_swapchain(swapchain_);
		vkb::destroy_surface(instance_, logicalDevice_.surface);
		vkb::destroy_device(logicalDevice_);
		vkb::destroy_instance(instance_);
	}

	VkQueue VulkanAPI::GetGraphicsQueue() const
	{
		return logicalDevice_.get_queue(vkb::QueueType::graphics).value();
	}

	std::shared_ptr<Pipeline> VulkanAPI::CreatePipeline(const Pipeline::Layout& _pipelineLayout)
	{
		return std::make_shared<VulkanPipeline>(logicalDevice_, physicalDevice_, _pipelineLayout);
	}

	std::shared_ptr<Mesh> VulkanAPI::CreateMesh(const Mesh::Layout& _meshLayout)
	{
		return std::make_shared<VulkanMesh>(logicalDevice_, physicalDevice_, *logicalDevice_.get_queue(vkb::QueueType::transfer), transferCommandPool_,  _meshLayout);
	}

	std::shared_ptr<Material> VulkanAPI::CreateMaterial()
	{
		VulkanMaterial::Initializer initializer{};
		initializer.logicalDevice_ = logicalDevice_;
		initializer.descriptorPool_ = CreateDescriptorPool(VulkanMaterial::GetDescriptorSetLayoutBindings());

		return std::make_shared<VulkanMaterial>(std::move(initializer));
	}

	std::shared_ptr<UniformBuffer> VulkanAPI::CreateUniformBuffer(const UniformBuffer::Layout& _layout)
	{
		return std::make_shared<VulkanUniformBuffer>(logicalDevice_, physicalDevice_, _layout);
	}

	std::shared_ptr<Texture> VulkanAPI::CreateTexture(const Texture::Layout& _textureLayout)
	{
		VulkanTexture::Initializer initializer{};
		initializer.logicalDevice_ = logicalDevice_;
		initializer.physicalDevice_ = physicalDevice_;
		initializer.graphicsQueue_ = *logicalDevice_.get_queue(vkb::QueueType::graphics);
		initializer.commandPool_ = commandPool_;

		return std::make_shared<VulkanTexture>(initializer, _textureLayout);
	}

	std::shared_ptr<RenderPass> VulkanAPI::CreateRenderPass()
	{
		return std::make_shared<VulkanRenderPass>();
	}

	uint32_t VulkanAPI::GetCurrentFrameIndex() const
	{
		return frameIndex_;
	}

	std::shared_ptr<RenderTarget> VulkanAPI::GetSwapchainRenderTarget() const
	{
		return swapchainRenderTargets_[swapchainImageIndex_];
	}

	VkExtent2D VulkanAPI::GetSwapchainExtent() const
	{
		return swapchain_.extent;
	}

	VkSemaphore VulkanAPI::GetImageAcquiringSemaphore() const
	{
		return frames_[frameIndex_].imageAcquiringSemaphore_;
	}

	VkSemaphore VulkanAPI::GetCommandExecutionSemaphore() const
	{
		return frames_[frameIndex_].commandExecutionSemaphore_;
	}

	VkFence VulkanAPI::GetFrameFence() const
	{
		return frames_[frameIndex_].frameFence_;
	}

	VkCommandBuffer VulkanAPI::AllocateCommnadBuffer() const
	{
		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
		VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.commandPool = commandPool_;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = 1;
		vkAllocateCommandBuffers(logicalDevice_, &commandBufferAllocateInfo, &commandBuffer) >> VulkanResultChecker::Get();

		return commandBuffer;
	}

	VkDescriptorSet VulkanAPI::AllocateDescriptorSet(VkDescriptorSetLayout _descriptorSetLayout) const
	{
		VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.descriptorPool = descriptorPool_;
		descriptorSetAllocateInfo.descriptorSetCount = 1;
		descriptorSetAllocateInfo.pSetLayouts = &_descriptorSetLayout;
		vkAllocateDescriptorSets(logicalDevice_, &descriptorSetAllocateInfo, &descriptorSet) >> VulkanResultChecker::Get();

		return descriptorSet;
	}

	bool VulkanAPI::WaitSwapchainImage()
	{
		Frame& currentFrame = frames_[frameIndex_];
		vkWaitForFences(logicalDevice_, 1, &currentFrame.frameFence_, VK_TRUE, UINT64_MAX);

		VkResult result = vkAcquireNextImageKHR(logicalDevice_, swapchain_, UINT64_MAX, currentFrame.imageAcquiringSemaphore_, VK_NULL_HANDLE, &swapchainImageIndex_);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapchain();
			return false;
		}

		vkResetFences(logicalDevice_, 1, &currentFrame.frameFence_) >> VulkanResultChecker::Get();
		return true;
	}

	void VulkanAPI::Present()
	{
		Frame& currentFrame = frames_[frameIndex_];

		VkSwapchainKHR swapchains[] = { swapchain_ };
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &currentFrame.commandExecutionSemaphore_;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapchains;
		presentInfo.pImageIndices = &swapchainImageIndex_;

		VkResult result = vkQueuePresentKHR(logicalDevice_.get_queue(vkb::QueueType::present).value(), &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapchain();
		}

		frameIndex_ = (frameIndex_ + 1) % config_.numFrameConcurrency_;
	}

	void VulkanAPI::WaitIdle()
	{
		vkDeviceWaitIdle(logicalDevice_);
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
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
		HINSTANCE instance = GetModuleHandle(nullptr);
		surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceCreateInfo.hinstance = instance;
		surfaceCreateInfo.hwnd = (HWND)_window;
		vkCreateWin32SurfaceKHR(instance_, &surfaceCreateInfo, nullptr, &surface) >> VulkanResultChecker::Get();

		VkPhysicalDeviceFeatures requiredFeatures{};
		requiredFeatures.samplerAnisotropy = true;

		vkb::PhysicalDeviceSelector deviceSelector(instance_);
		deviceSelector.set_surface(surface);
		deviceSelector.set_required_features(requiredFeatures);
		physicalDevice_ = Build(deviceSelector, &vkb::PhysicalDeviceSelector::select, vkb::DeviceSelectionMode::partially_and_fully_suitable);

		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(physicalDevice_, &properties);
	}

	void VulkanAPI::CreateLogicalDevice()
	{
		vkb::DeviceBuilder deviceBuilder(physicalDevice_);
		logicalDevice_ = Build(deviceBuilder, &vkb::DeviceBuilder::build);
	}

	void VulkanAPI::CreateSwapchain()
	{
		vkb::SwapchainBuilder swapchainBuilder(logicalDevice_);
		VkSurfaceFormatKHR desiredFormat{};
		desiredFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		desiredFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
		swapchainBuilder.set_desired_format(desiredFormat);
		swapchainBuilder.use_default_image_usage_flags();
		swapchainBuilder.use_default_present_mode_selection();
		swapchain_ = Build(swapchainBuilder, &vkb::SwapchainBuilder::build);
	}

	void VulkanAPI::CreateCommandPools()
	{
		VkCommandPoolCreateInfo commandPoolCreateInfo{};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = logicalDevice_.get_queue_index(vkb::QueueType::graphics).value();
		vkCreateCommandPool(logicalDevice_, &commandPoolCreateInfo, nullptr, &commandPool_) >> VulkanResultChecker::Get();

		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		commandPoolCreateInfo.queueFamilyIndex = logicalDevice_.get_queue_index(vkb::QueueType::transfer).value();
		vkCreateCommandPool(logicalDevice_, &commandPoolCreateInfo, nullptr, &transferCommandPool_) >> VulkanResultChecker::Get();
	}

    void VulkanAPI::CreateDescriptorPool()
    {
		std::vector<VkDescriptorPoolSize> poolSizes;
		VkDescriptorPoolSize poolSize{};
		poolSize.type = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSize.descriptorCount = config_.numMaxSamplers_;
		poolSizes.push_back(poolSize);
		poolSize.type = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = config_.numMaxUBuffers_;
		poolSizes.push_back(poolSize);

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.maxSets = config_.numMaxDescriptorSets_;
		descriptorPoolCreateInfo.poolSizeCount = (uint32_t)poolSizes.size();
		descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();
		vkCreateDescriptorPool(logicalDevice_, &descriptorPoolCreateInfo, nullptr, &descriptorPool_);
    }

	void VulkanAPI::CreateSyncObjects()
	{
		VkSemaphoreCreateInfo semaphoreCreateInfo{};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceCreateInfo{};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (Frame& frame : frames_)
		{
			vkCreateSemaphore(logicalDevice_, &semaphoreCreateInfo, nullptr, &frame.imageAcquiringSemaphore_);
			vkCreateSemaphore(logicalDevice_, &semaphoreCreateInfo, nullptr, &frame.commandExecutionSemaphore_);

			vkCreateFence(logicalDevice_, &fenceCreateInfo, nullptr, &frame.frameFence_);
		}
	}

	void VulkanAPI::CreateSwapchainRenderTargets()
	{
		swapchainRenderTargets_.clear();

		for (VkImageView swapchainImageView : swapchain_.get_image_views().value())
		{
			auto renderTarget = std::make_shared<VulkanRenderTarget>(logicalDevice_, swapchain_.extent.width, swapchain_.extent.height, swapchainImageView);
			ShaderDescriptor::Output depthStencilDescription{};
			depthStencilDescription.width_ = swapchain_.extent.width;
			depthStencilDescription.height_ = swapchain_.extent.height;
			depthStencilDescription.format_ = ImageFormat::D32_SFLOAT_U8_UINT;
			depthStencilDescription.usage_ = ImageUsage::DEPTH_STENCIL;
			renderTarget->AddAttachment(physicalDevice_, depthStencilDescription);
			swapchainRenderTargets_.push_back(std::move(renderTarget));
		}
	}

	void VulkanAPI::RecreateSwapchain()
	{
		vkDeviceWaitIdle(logicalDevice_);

		vkb::destroy_swapchain(swapchain_);

		CreateSwapchain();
		CreateSwapchainRenderTargets();
	}

	VkDescriptorPool VulkanAPI::CreateDescriptorPool(const std::vector<VkDescriptorSetLayoutBinding>& _bindings) const
	{
		VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
		std::vector<VkDescriptorPoolSize> poolSizes;

		uint32_t numSamplers = 0;
		uint32_t numUniformBuffers = 0;

		for (const auto& binding : _bindings)
		{
			if (binding.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
			{
				numSamplers++;
			}
			else if (binding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
			{
				numUniformBuffers++;
			}
		}

		VkDescriptorPoolSize poolSize{};
		poolSize.type = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSize.descriptorCount = numSamplers;
		if (poolSize.descriptorCount != 0)
		{
			poolSizes.push_back(poolSize);
		}
		poolSize.type = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = numUniformBuffers;
		if (poolSize.descriptorCount != 0)
		{
			poolSizes.push_back(poolSize);
		}

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.maxSets = 1;
		descriptorPoolCreateInfo.poolSizeCount = (uint32_t)poolSizes.size();
		descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();

		vkCreateDescriptorPool(logicalDevice_, &descriptorPoolCreateInfo, nullptr, &descriptorPool);
		return descriptorPool;
	}
}
