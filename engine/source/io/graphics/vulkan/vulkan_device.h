#pragma once
#include <vulkan/vulkan.h>
#include "utility/stl.h"

namespace io::graphics
{
	class VulkanSwapChain;
	class VulkanPipeline;

	class VulkanDevice
	{
	private:
		VkPhysicalDevice physicalDevice_;
		VkDevice logicalDevice_;

		uint32_t graphicsQueueIndex_;
		uint32_t presentQueueIndex_;

		VkCommandPool commandPool_;
		VkRenderPass renderPass_;

		VkQueue graphicsQueue_;
		VkQueue presentQueue_;

		static constexpr size_t frameConcurrency = 2;
		std::vector<VkCommandBuffer> commandBuffers_;
		std::vector<VkSemaphore> imageAvailableSemaphores_;
		std::vector<VkSemaphore> renderFinishedSemaphores_;
		std::vector<VkFence> fences_;

		size_t currentFrame_ = 0;
		uint32_t currentImageIndex_ = 0;

	public:
		void Initialize(VkInstance _instance, VkSurfaceKHR _surface);
		void Release();

		void BeginCommandBuffer(const VulkanSwapChain& _swapChain);
		void BeginRenderPass(const VulkanSwapChain& _swapChain);
		void Execute(const VulkanDevice& _device, const VulkanPipeline& _pipeline);
		void Present(const VulkanDevice& _device, const VulkanSwapChain& _swapChain);

		VkPhysicalDevice GetPhysicalDevice() const;
		VkDevice GetLogicalDevice() const;
		VkRenderPass GetRenderPass() const;

		uint32_t GetGraphicsQueueFamilyIndex() const;
		uint32_t GetPresentQueueFamilyIndex() const;

		VkBuffer CreateBuffer(const VulkanDevice& _device, size_t _size, VkBufferUsageFlags _usage) const;
		VkDeviceMemory AllocateMemory(const VulkanDevice& _device, VkBuffer _buffer, VkMemoryPropertyFlags _properties) const;
		void CopyBuffer(const VulkanDevice& _device, VkBuffer _source, VkBuffer _destination, size_t _size) const;

	private:
		uint32_t AcquireSwapChainImage(const VulkanSwapChain& _swapChain);
		void RecordCommandBuffer(const VulkanPipeline& _pipeline) const;
		void SubmitCommand();
	};
}