#include "vulkan_uniform_buffer.h"
#include "vulkan_result.hpp"
#include "vulkan_utility.h"
#include "vulkan_shader_binding.h"

namespace graphics
{
	class VulkanUniformBufferBinding : public VulkanShaderBinding
	{
	public:
		VkDescriptorBufferInfo bufferInfo_{};

	public:
		virtual void FillBindingInfo(VkWriteDescriptorSet& _WriteDescriptorSet) const override
		{
			_WriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			_WriteDescriptorSet.pBufferInfo = &bufferInfo_;
		}
	};

	VulkanUniformBuffer::VulkanUniformBuffer(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, const UniformBuffer::Layout& _layout)
		: logicalDevice_(_logicalDevice)
		, bufferSize_(_layout.size_)
		, persistentMapping_(_layout.persistentMapping_)
	{
		VkBufferUsageFlags memoryFlag = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		CreateBuffer(_logicalDevice, _physicalDevice, memoryFlag, properties, bufferSize_, buffer_, bufferMemory_);

		if (persistentMapping_)
		{
			vkMapMemory(logicalDevice_, bufferMemory_, 0, bufferSize_, 0, &mapped_) >> VulkanResultChecker::Get();
		}

		bufferInfo_.buffer = buffer_;
		bufferInfo_.offset = 0;
		bufferInfo_.range = bufferSize_;

		bindingImpl_ = std::make_shared<VulkanUniformBufferBinding>();
		bindingImpl_->bufferInfo_ = bufferInfo_;
	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		if (persistentMapping_)
		{
			vkUnmapMemory(logicalDevice_, bufferMemory_);
		}

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

	std::shared_ptr<ShaderBinding::BindingImpl> VulkanUniformBuffer::GetBindingImpl() const
	{
		return bindingImpl_;
	}

    void VulkanUniformBuffer::Update(const void* _data)
	{
		if (persistentMapping_)
		{
			memcpy(mapped_, _data, bufferSize_);
		}
		else
		{
			vkMapMemory(logicalDevice_, bufferMemory_, 0, bufferSize_, 0, &mapped_) >> VulkanResultChecker::Get();
			memcpy(mapped_, _data, bufferSize_);
			vkUnmapMemory(logicalDevice_, bufferMemory_);
		}
	}
}