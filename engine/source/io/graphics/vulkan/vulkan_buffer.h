#pragma once
#include <vulkan/vulkan.h>

namespace io::graphics
{
	uint32_t FindMemoryTypeIndex(VkPhysicalDevice _physicalDevice, VkMemoryRequirements _memoryRequirements, VkMemoryPropertyFlags _requiredProperties);
	void CreateBuffer(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _properties, VkDeviceSize _bufferSize, VkBuffer& _outBuffer, VkDeviceMemory& _outBufferMemory);
	void CopyBuffer(VkDevice _logicalDevice, VkQueue _transferQueue, VkCommandPool _commandPool, VkBuffer _srcBuffer, VkBuffer _dstBuffer, VkDeviceSize _bufferSize);
}