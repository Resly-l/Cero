#include "vulkan_commander.h"
#include "vulkan_device.h"
#include "vulkan_swapchain.h"
#include "vulkan_pipeline.h"

namespace io::graphics
{
	void VulkanCommander::Intialize(const VulkanDevice& _device)
	{
		vkGetDeviceQueue(_device.GetLogicalDevice(), _device.GetGraphicsQueueFamilyIndex(), 0, &graphicsQueue_);
		vkGetDeviceQueue(_device.GetLogicalDevice(), _device.GetPresentQueueFamilyIndex(), 0, &presentQueue_);

		CreateCommandPool(_device);
		CreateCommandBuffer(_device);
		CreateSyncObjects(_device);
	}

	void VulkanCommander::Release(const VulkanDevice& _device)
	{
		for (size_t i = 0; i < frameConcurrency; i++)
		{
			vkDestroySemaphore(_device.GetLogicalDevice(), imageAvailableSemaphores_[i], nullptr);
			vkDestroySemaphore(_device.GetLogicalDevice(), renderFinishedSemaphores_[i], nullptr);
			vkDestroyFence(_device.GetLogicalDevice(), inFlightFences_[i], nullptr);
		}

		vkDestroyCommandPool(_device.GetLogicalDevice(), commandPool_, nullptr);
	}

	void VulkanCommander::Execute(const VulkanDevice& _device, const VulkanSwapChain& _swapChain, const VulkanPipeline& _pipeline)
	{
		currentImageIndex_ = AcquireSwapChainImage(_device, _swapChain);
		RecordCommandBuffer(_swapChain, _pipeline, currentImageIndex_);
		SubmitCommand();
	}

	void VulkanCommander::Present(const VulkanDevice& _device, const VulkanSwapChain& _swapChain)
	{
		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores_[currentFrame_] };
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { _swapChain.GetInstance() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &currentImageIndex_;
		presentInfo.pResults = nullptr; // Optional

		vkQueuePresentKHR(presentQueue_, &presentInfo);

		currentFrame_ = (currentFrame_ + 1) % frameConcurrency;
	}

	VkBuffer VulkanCommander::CreateBuffer(const VulkanDevice& _device, size_t _size, VkBufferUsageFlags _usage) const
	{
		VkBuffer buffer;
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = _size;
        bufferInfo.usage = _usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(_device.GetLogicalDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
		{
            throw std::runtime_error("failed to create buffer");
        }

		return buffer;
	}

	VkDeviceMemory VulkanCommander::AllocateMemory(const VulkanDevice& _device, VkBuffer _buffer, VkMemoryPropertyFlags _properties) const
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
        vkGetBufferMemoryRequirements(_device.GetLogicalDevice(), _buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, _properties);

        if (vkAllocateMemory(_device.GetLogicalDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
		{
            throw std::runtime_error("failed to allocate buffer memory");
        }

        vkBindBufferMemory(_device.GetLogicalDevice(), _buffer, bufferMemory, 0);
		return bufferMemory;
	}

	void VulkanCommander::CopyBuffer(const VulkanDevice& _device, VkBuffer _source, VkBuffer _destination, size_t _size) const
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool_;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(_device.GetLogicalDevice(), &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		VkBufferCopy copyRegion{};
		copyRegion.size = _size;
		vkCmdCopyBuffer(commandBuffer, _source, _destination, 1, &copyRegion);

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueue_);

		vkFreeCommandBuffers(_device.GetLogicalDevice(), commandPool_, 1, &commandBuffer);
	}

	void VulkanCommander::CreateCommandPool(const VulkanDevice& _device)
	{
		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = _device.GetGraphicsQueueFamilyIndex();

		if (vkCreateCommandPool(_device.GetLogicalDevice(), &poolInfo, nullptr, &commandPool_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create command pool!");
		}
	}

	void VulkanCommander::CreateCommandBuffer(const VulkanDevice& _device)
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool_;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = frameConcurrency;

		if (vkAllocateCommandBuffers(_device.GetLogicalDevice(), &allocInfo, commandBuffers_.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffers");
		}
	}

	void VulkanCommander::CreateSyncObjects(const VulkanDevice& _device)
	{
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < frameConcurrency; i++)
		{
			if (vkCreateSemaphore(_device.GetLogicalDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores_[i]) != VK_SUCCESS
				|| vkCreateSemaphore(_device.GetLogicalDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores_[i]) != VK_SUCCESS
				|| vkCreateFence(_device.GetLogicalDevice(), &fenceInfo, nullptr, &inFlightFences_[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create sync objects");
			}
		}
	}

	uint32_t VulkanCommander::AcquireSwapChainImage(const VulkanDevice& _device, const VulkanSwapChain& _swapChain)
	{
		vkWaitForFences(_device.GetLogicalDevice(), 1, &inFlightFences_[currentFrame_], VK_TRUE, UINT64_MAX);
		vkResetFences(_device.GetLogicalDevice(), 1, &inFlightFences_[currentFrame_]);

		uint32_t imageIndex = 0;
		if (vkAcquireNextImageKHR(_device.GetLogicalDevice(), _swapChain.GetInstance(), UINT64_MAX, imageAvailableSemaphores_[currentFrame_], VK_NULL_HANDLE, &imageIndex) != VK_SUCCESS)
		{
			throw std::exception("failed to get avilable swap chain index");
		}

		vkResetCommandBuffer(commandBuffers_[currentFrame_], 0);

		return imageIndex;
	}

	void VulkanCommander::RecordCommandBuffer(const VulkanSwapChain& _swapChain, const VulkanPipeline& _pipeline, const size_t _bufferIndex) const
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(commandBuffers_[currentFrame_], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = _swapChain.GetRenderPass();
		renderPassInfo.framebuffer = _swapChain.GetFrameBuffers()[_bufferIndex];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = { _swapChain.GetWidth(), _swapChain.GetHeight() };
		VkClearValue clearColor{ 0.01f, 0.01f, 0.01f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffers_[currentFrame_], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffers_[currentFrame_], VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline.GetInstance());

		VkBuffer vertexBuffers[] = { _pipeline.GetVertexBuffer() };
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(commandBuffers_[currentFrame_], 0, 1, vertexBuffers, offsets);

		vkCmdDraw(commandBuffers_[currentFrame_], 3, 1, 0, 0);
		vkCmdEndRenderPass(commandBuffers_[currentFrame_]);

		if (vkEndCommandBuffer(commandBuffers_[currentFrame_]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}
	}

	void VulkanCommander::SubmitCommand()
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

		if (vkQueueSubmit(graphicsQueue_, 1, &submitInfo, inFlightFences_[currentFrame_]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to submit draw command buffer");
		}
	}
}