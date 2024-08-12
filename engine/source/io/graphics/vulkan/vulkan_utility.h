#pragma once
#include <vulkan/vulkan.h>
#include "io/graphics/pipeline.h"

namespace io::graphics
{
	uint32_t FindMemoryTypeIndex(VkPhysicalDevice _physicalDevice, VkMemoryRequirements _memoryRequirements, VkMemoryPropertyFlags _requiredProperties);
	void CreateBuffer(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _properties, VkDeviceSize _bufferSize, VkBuffer& _outBuffer, VkDeviceMemory& _outBufferMemory);
	void CopyBuffer(VkDevice _logicalDevice, VkQueue _transferQueue, VkCommandPool _commandPool, VkBuffer _srcBuffer, VkBuffer _dstBuffer, VkDeviceSize _bufferSize);

	class VulkanTypeConverter
	{
	public:
		static VkPrimitiveTopology Convert(PrimitiveTopology _topology);
		static VkFormat Convert(ImageFormat _format);
		static VkImageUsageFlags Convert(ImageUsage _usage);
		static VkAttachmentLoadOp ConvertLoadOp(ImageOperation _operation);
		static VkAttachmentStoreOp ConvertStoreOp(ImageOperation _operation);
		static VkShaderStageFlags Convert(ShaderBinding::Stage _stage);
	};
}