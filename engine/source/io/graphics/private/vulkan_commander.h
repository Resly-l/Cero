#pragma once
#include <vulkan/vulkan.h>
#include "utility/stl.h"

namespace io::graphics
{
	class VulkanDevice;
	class VulkanSwapChain;
	class VulkanPipeline;

	class VulkanCommander
	{
	private:
		static constexpr size_t frameConcurrency = 2;

	private:
		VkCommandPool commandPool_;

		VkQueue graphicsQueue_;
		VkQueue presentQueue_;

		std::array<VkCommandBuffer, frameConcurrency> commandBuffers_;
		std::array<VkSemaphore, frameConcurrency> imageAvailableSemaphores_;
		std::array<VkSemaphore, frameConcurrency> renderFinishedSemaphores_;
		std::array<VkFence, frameConcurrency> inFlightFences_;

		size_t currentFrame_ = 0;
		uint32_t currentImageIndex_ = 0;

	public:
		void Intialize(const VulkanDevice& _device);
		void Release(const VulkanDevice& _device);

		void Execute(const VulkanDevice& _device, const VulkanSwapChain& _swapChain, const VulkanPipeline& _pipeline);
		void Present(const VulkanDevice& _device, const VulkanSwapChain& _swapChain);

		VkBuffer CreateBuffer(const VulkanDevice& _device, size_t _size, VkBufferUsageFlags _usage) const;
		VkDeviceMemory AllocateMemory(const VulkanDevice& _device, VkBuffer _buffer, VkMemoryPropertyFlags _properties) const;
		void CopyBuffer(const VulkanDevice& _device, VkBuffer _source, VkBuffer _destination, size_t _size) const;

	private:
		void CreateCommandPool(const VulkanDevice& _device);
		void CreateCommandBuffer(const VulkanDevice& _device);
		void CreateSyncObjects(const VulkanDevice& _device);

		uint32_t AcquireSwapChainImage(const VulkanDevice& _device, const VulkanSwapChain& _swapChain);
		void RecordCommandBuffer(const VulkanSwapChain& _swapChain, const VulkanPipeline& _pipeline, const size_t _bufferIndex) const;
		void SubmitCommand();
	};
}