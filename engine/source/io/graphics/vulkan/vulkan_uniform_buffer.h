#pragma once
#include <vulkan/vulkan.h>

namespace io::graphics
{
	class VulkanUniformBuffer
	{
	private:
		VkDevice logicalDevice_;
		VkBuffer buffer_;
		VkDeviceMemory bufferMemory_;
		void* mapped_ = nullptr;

		uint32_t bufferSize_;

	public:
		VulkanUniformBuffer(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, uint32_t _bufferSize, void* _data);
		~VulkanUniformBuffer();

	public:
		VkBuffer GetBuffer() const;
		uint32_t GetBufferSize() const;
		void UpdateBuffer(void* _data);
	};
}