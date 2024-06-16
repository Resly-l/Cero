#include "vulkan_uniform_buffer.h"
#include "vulkan_buffer.h"
#include "vulkan_validation.hpp"

namespace io::graphics
{
	VulkanUniformBuffer::VulkanUniformBuffer(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, VkDeviceSize _bufferSize, uint32_t _index)
		: logicalDevice_(_logicalDevice)
		, bufferSize_(_bufferSize)
		, index_(_index)
	{
		VkBufferUsageFlags memoryFlag = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		CreateBuffer(_logicalDevice, _physicalDevice, memoryFlag, properties, _bufferSize, buffer_, bufferMemory_);
		vkMapMemory(logicalDevice_, bufferMemory_, 0, bufferSize_, 0, &mapped_) >> VulkanResultChecker::GetInstance();
	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		vkFreeMemory(logicalDevice_, bufferMemory_, nullptr);
		vkDestroyBuffer(logicalDevice_, buffer_, nullptr);
	}

	VkBuffer VulkanUniformBuffer::GetBuffer() const
	{
		return buffer_;
	}

	VkDeviceSize VulkanUniformBuffer::GetBufferSize() const
	{
		return bufferSize_;
	}

	uint32_t VulkanUniformBuffer::GetIndex() const
	{
		return index_;
	}

	void VulkanUniformBuffer::UpdateBuffer(const void* _data)
	{
		memcpy(mapped_, _data, bufferSize_);
	}
}