#include "vulkan_device.h"
#include "vulkan_swapchain.h"
#include "vulkan_pipeline.h"
#include "vulkan_bootstrap.h"
#include "utility/stl.h"

namespace io::graphics
{
	void VulkanDevice::Initialize(VkInstance _instance, VkSurfaceKHR _surface)
	{
		physicalDevice_ = VulkanBootstrap::SelectPhysicalDevice(_instance);

		graphicsQueueIndex_ = VulkanBootstrap::FindGraphicsQueueIndex(physicalDevice_, _surface);
		presentQueueIndex_ = VulkanBootstrap::FindPresentQueueIndex(physicalDevice_, _surface);

		logicalDevice_ = VulkanBootstrap::CreateLogicalDevice(physicalDevice_, _surface);

		vkGetDeviceQueue(logicalDevice_, graphicsQueueIndex_, 0, &graphicsQueue_);
		vkGetDeviceQueue(logicalDevice_, presentQueueIndex_, 0, &presentQueue_);

		commandPool_ = VulkanBootstrap::CreateCommandPool(logicalDevice_, graphicsQueueIndex_);
		commandBuffers_ = VulkanBootstrap::CreateCommandBuffers(logicalDevice_, commandPool_, frameConcurrency);
		renderPass_ = VulkanBootstrap::CreateRenderPass(logicalDevice_, VulkanBootstrap::FindBestSurfaceFormat(physicalDevice_, _surface));
		
		imageAvailableSemaphores_ = VulkanBootstrap::CreateSemaphores(logicalDevice_, frameConcurrency);
		renderFinishedSemaphores_ = VulkanBootstrap::CreateSemaphores(logicalDevice_, frameConcurrency);
		fences_ = VulkanBootstrap::CreateFences(logicalDevice_, frameConcurrency);
	}

	void VulkanDevice::Release()
	{
		vkDestroyRenderPass(logicalDevice_, renderPass_, nullptr);

		for (size_t i = 0; i < frameConcurrency; i++)
		{
			vkDestroySemaphore(logicalDevice_, imageAvailableSemaphores_[i], nullptr);
			vkDestroySemaphore(logicalDevice_, renderFinishedSemaphores_[i], nullptr);
			vkDestroyFence(logicalDevice_, fences_[i], nullptr);
		}

		vkDestroyCommandPool(logicalDevice_, commandPool_, nullptr);

		vkDestroyDevice(logicalDevice_, nullptr);
	}

	void VulkanDevice::BeginCommandBuffer(const VulkanSwapChain& _swapChain)
	{
		currentImageIndex_ = AcquireSwapChainImage(_swapChain);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(commandBuffers_[currentFrame_], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer");
		}
	}
	
	void VulkanDevice::BeginRenderPass(const VulkanSwapChain& _swapChain)
	{
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass_;
		renderPassInfo.framebuffer = _swapChain.GetFrameBuffers()[currentImageIndex_];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = { _swapChain.GetWidth(), _swapChain.GetHeight() };
		VkClearValue clearColor{ 0.0f, 0.0f, 0.0f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffers_[currentFrame_], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanDevice::Execute(const VulkanDevice& _device, const VulkanPipeline& _pipeline)
	{
		RecordCommandBuffer(_pipeline);
		SubmitCommand();
	}
	
	void VulkanDevice::Present(const VulkanDevice& _device, const VulkanSwapChain& _swapChain)
	{
		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores_[currentFrame_] };
		VkSwapchainKHR swapChains[] = { _swapChain.GetInstance() };

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &currentImageIndex_;
		presentInfo.pResults = nullptr; // Optional

		vkQueuePresentKHR(presentQueue_, &presentInfo);

		currentFrame_ = (currentFrame_ + 1) % frameConcurrency;
	}

	VkPhysicalDevice VulkanDevice::GetPhysicalDevice() const
	{
		return physicalDevice_;
	}

	VkDevice VulkanDevice::GetLogicalDevice() const
	{
		return logicalDevice_;
	}

	VkRenderPass VulkanDevice::GetRenderPass() const
	{
		return renderPass_;
	}

	uint32_t VulkanDevice::GetGraphicsQueueFamilyIndex() const
	{
		return graphicsQueueIndex_;
	}

	uint32_t VulkanDevice::GetPresentQueueFamilyIndex() const
	{
		return presentQueueIndex_;
	}

	VkBuffer VulkanDevice::CreateBuffer(const VulkanDevice& _device, size_t _size, VkBufferUsageFlags _usage) const
	{
		VkBuffer buffer;
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = _size;
        bufferInfo.usage = _usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(logicalDevice_, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
		{
            throw std::runtime_error("failed to create buffer");
        }

		return buffer;
	}

	VkDeviceMemory VulkanDevice::AllocateMemory(const VulkanDevice& _device, VkBuffer _buffer, VkMemoryPropertyFlags _properties) const
	{
		auto FindMemoryType = [&](uint32_t _typeFilter, VkMemoryPropertyFlags _flags)
		{
			VkPhysicalDeviceMemoryProperties memProperties;
			vkGetPhysicalDeviceMemoryProperties(_device.GetPhysicalDevice(), &memProperties);

			for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
			{
				if ((_typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & _flags) == _flags)
				{
					return i;
				}
			}

			throw std::runtime_error("failed to find suitable memory type!");
		};

		VkDeviceMemory bufferMemory;
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(logicalDevice_, _buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, _properties);

        if (vkAllocateMemory(logicalDevice_, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
		{
            throw std::runtime_error("failed to allocate buffer memory");
        }

        vkBindBufferMemory(logicalDevice_, _buffer, bufferMemory, 0);
		return bufferMemory;
	}

	void VulkanDevice::CopyBuffer(const VulkanDevice& _device, VkBuffer _source, VkBuffer _destination, size_t _size) const
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool_;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(logicalDevice_, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);
		{
			VkBufferCopy copyRegion{};
			copyRegion.size = _size;
			vkCmdCopyBuffer(commandBuffer, _source, _destination, 1, &copyRegion);
		}
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueue_);

		vkFreeCommandBuffers(logicalDevice_, commandPool_, 1, &commandBuffer);
	}

	uint32_t VulkanDevice::AcquireSwapChainImage(const VulkanSwapChain& _swapChain)
	{
		vkWaitForFences(logicalDevice_, 1, &fences_[currentFrame_], VK_TRUE, UINT64_MAX);

		uint32_t imageIndex = 0;
		if (vkAcquireNextImageKHR(logicalDevice_, _swapChain.GetInstance(), UINT64_MAX, imageAvailableSemaphores_[currentFrame_], VK_NULL_HANDLE, &imageIndex) != VK_SUCCESS)
		{
			throw std::exception("failed to get avilable swap chain index");
		}

		vkResetFences(logicalDevice_, 1, &fences_[currentFrame_]);
		vkResetCommandBuffer(commandBuffers_[currentFrame_], 0);

		return imageIndex;
	}

	void VulkanDevice::RecordCommandBuffer(const VulkanPipeline& _pipeline) const
	{
		vkCmdBindPipeline(commandBuffers_[currentFrame_], VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline.GetInstance());

		VkBuffer vertexBuffers[] = { _pipeline.GetVertexBuffer() };
		//VkBuffer indexBuffers[] = { _pipeline.GetIndexBuffer() };
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(commandBuffers_[currentFrame_], 0, 1, vertexBuffers, offsets);
		//vkCmdBindIndexBuffer(commandBuffers_[currentFrame_], 0, 1, indexBuffers, offsets, VK_INDEX_TYPE_UINT32);

		vkCmdDraw(commandBuffers_[currentFrame_], 3, 1, 0, 0);
		vkCmdEndRenderPass(commandBuffers_[currentFrame_]);

		if (vkEndCommandBuffer(commandBuffers_[currentFrame_]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer");
		}
	}

	void VulkanDevice::SubmitCommand()
	{
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { imageAvailableSemaphores_[currentFrame_] };
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
			throw std::runtime_error("failed to submit draw command buffer");
		}
	}
}