#include "vulkan_uniform_buffer.h"
#include "vulkan_buffer.h"
#include "vulkan_result.hpp"
#include "vulkan_utility.h"

namespace io::graphics
{
	VulkanUniformBuffer::VulkanUniformBuffer(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, const UniformBuffer::Layout& _layout)
		: logicalDevice_(_logicalDevice)
		, bufferSize_(_layout.size_)
	{
		slot_ = _layout.slot_;
		numElements_ = _layout.numElements_;
		stage_ = VulkanTypeConverter::Convert(_layout.stage_);

		VkBufferUsageFlags memoryFlag = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		CreateBuffer(_logicalDevice, _physicalDevice, memoryFlag, properties, bufferSize_, buffer_, bufferMemory_);
		vkMapMemory(logicalDevice_, bufferMemory_, 0, bufferSize_, 0, &mapped_) >> VulkanResultChecker::GetInstance();

		bufferInfo_.buffer = buffer_;
		bufferInfo_.offset = 0;
		bufferInfo_.range = bufferSize_;
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

	uint32_t VulkanUniformBuffer::GetSlot() const
	{
		return slot_;
	}

	class VulkanUnformBufferBinding : public VulkanShaderBinding
	{
	public:
		uint32_t slot_ = 0;
		uint32_t numElements_ = 1;
		VkShaderStageFlags stage_{};
		VkDescriptorBufferInfo bufferInfo_{};

	public:
		virtual VkDescriptorSetLayoutBinding GetDescriptorLayout() const override
		{
			VkDescriptorSetLayoutBinding layout{};
			layout.binding = slot_;
			layout.descriptorCount = numElements_;
			layout.pImmutableSamplers = nullptr;
			layout.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			layout.stageFlags = stage_;
			return layout;
		}

		virtual VkWriteDescriptorSet GetDescriptorWrite(VkDescriptorSet _descriptorSet) const override
		{
			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = _descriptorSet;
			descriptorWrite.dstBinding = slot_;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo_;
			descriptorWrite.pImageInfo = nullptr;
			descriptorWrite.pTexelBufferView = nullptr;
			return descriptorWrite;
		}
	};

    std::shared_ptr<ShaderBinding> VulkanUniformBuffer::GetShaderBinding() const
    {
		auto uniformBufferBinding = std::make_shared<VulkanUnformBufferBinding>();
		uniformBufferBinding->slot_ = slot_;
		uniformBufferBinding->numElements_ = numElements_;
		uniformBufferBinding->stage_ = stage_;
		uniformBufferBinding->bufferInfo_ = bufferInfo_;

		return uniformBufferBinding;
    }

    void VulkanUniformBuffer::Update(const void* _data) const
	{
		memcpy(mapped_, _data, bufferSize_);
	}
}