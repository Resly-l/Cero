#pragma once
#include <vulkan/vulkan.h>
#include "io/graphics/shader.h"

namespace io::graphics
{
	class VulkanShaderBinding
	{
	protected:
		uint32_t slot_ = 0;
		uint32_t numElements_ = 0;
		VkShaderStageFlags stage_{};

	public:
		virtual ~VulkanShaderBinding() {}

	public:
		virtual VkDescriptorSetLayoutBinding GetDescriptorLayout() const = 0;
		virtual VkWriteDescriptorSet GetDescriptorWrite(VkDescriptorSet _descriptorSet) const = 0;
	};
}