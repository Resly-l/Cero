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
		VkDeviceSize bufferSize_;
		uint32_t index_;
		void* mapped_ = nullptr;

	public:
		VulkanUniformBuffer(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, VkDeviceSize _bufferSize, uint32_t _index);
		~VulkanUniformBuffer();

	public:
		VkBuffer GetBuffer() const;
		VkDeviceSize GetBufferSize() const;
		uint32_t GetIndex() const;
		void UpdateBuffer(const void* _data);
	};
}