#pragma once
#include "vulkan/vulkan.h"
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
		const bool persistentMapping_ = false;
		VkDescriptorBufferInfo bufferInfo_{};
		std::shared_ptr<class VulkanUniformBufferBinding> bindingImpl_;

	public:
		VulkanUniformBuffer(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, const UniformBuffer::Layout& _layout);
		~VulkanUniformBuffer();

	public:
		virtual std::shared_ptr<ShaderBinding::BindingImpl> GetBindingImpl() const override;

		virtual void Update(const void* _data) override;

		VkBuffer GetBuffer() const;
		VkDeviceSize GetBufferSize() const;
	};
}