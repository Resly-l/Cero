#pragma once
#include <vulkan/vulkan.h>

namespace io::graphics
{
	void CreateBuffer(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _properties, uint32_t _bufferSize, VkBuffer& _outBuffer, VkDeviceMemory& _outBufferMemory);
	void CopyBuffer(VkDevice _logicalDevice, VkQueue _transferQueue, VkCommandPool _commandPool, VkBuffer _srcBuffer, VkBuffer _dstBuffer, uint32_t _bufferSize);
}