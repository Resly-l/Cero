#pragma once
#include <vulkan/vulkan.h>
#include "io/graphics/graphics_pipeline.h"

namespace io::graphics
{
	class VulkanTypeConverter
	{
	public:
		static VkFormat Convert(ImageFormat _format);
		static VkImageUsageFlags Convert(ImageUsage _usage);
		static VkImageAspectFlags GetAspectMask(VkFormat _format, VkImageUsageFlags _usage);
	};

	void CreateBuffer(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _properties,  uint32_t _bufferSize, VkBuffer& _outBuffer, VkDeviceMemory& _outBufferMemory);
	void CopyBuffer(VkDevice _logicalDevice, VkQueue _transferQueue, VkCommandPool _commandPool, VkBuffer _srcBuffer, VkBuffer _dstBuffer, uint32_t _bufferSize);
}