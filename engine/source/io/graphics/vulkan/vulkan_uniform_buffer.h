#pragma once
#include "vulkan_shader_binding.h"
#include "io/graphics/uniform_buffer.h"

namespace io::graphics
{
	class VulkanUniformBuffer : public UniformBuffer, public VulkanShaderBinding
	{
	private:
		VkDevice logicalDevice_ = VK_NULL_HANDLE;
		VkBuffer buffer_ = VK_NULL_HANDLE;
		VkDeviceMemory bufferMemory_ = VK_NULL_HANDLE;
		VkDeviceSize bufferSize_{};
		void* mapped_ = nullptr;
		VkDescriptorBufferInfo bufferInfo_{};

	public:
		VulkanUniformBuffer(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, const UniformBuffer::Layout& _layout);
		~VulkanUniformBuffer();

	public:
		virtual VkDescriptorSetLayoutBinding GetDescriptorLayout() const override;
		virtual VkWriteDescriptorSet GetDescriptorWrite(VkDescriptorSet _descriptorSet) const override;
		virtual void Update(const void* _data) const override;

		VkBuffer GetBuffer() const;
		VkDeviceSize GetBufferSize() const;
		uint32_t GetSlot() const;
	};
}