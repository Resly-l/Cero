#pragma once
#include "vulkan_shader_binding.h"
#include "graphics/uniform_buffer.h"

namespace graphics
{
	class VulkanUniformBuffer : public UniformBuffer
	{
	private:
		VkDevice logicalDevice_ = VK_NULL_HANDLE;
		VkBuffer buffer_ = VK_NULL_HANDLE;
		VkDeviceMemory bufferMemory_ = VK_NULL_HANDLE;
		VkDeviceSize bufferSize_{};
		void* mapped_ = nullptr;
		VkDescriptorBufferInfo bufferInfo_{};
		VkShaderStageFlags stage_{};

	public:
		VulkanUniformBuffer(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, const UniformBuffer::Layout& _layout);
		~VulkanUniformBuffer();

	public:
		virtual void Update(const void* _data) const override;
		virtual std::shared_ptr<ShaderBinding::ApiSpecificImpl> GetApiSpecificImpl() const override;

		VkBuffer GetBuffer() const;
		VkDeviceSize GetBufferSize() const;
	};
}