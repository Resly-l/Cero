#pragma once
#include <vulkan/vulkan.h>
#include "io/graphics/shader.h"

namespace io::graphics
{
	class VulkanShaderBinding : public ShaderBinding
	{
	public:
		virtual ~VulkanShaderBinding() {}

	public:
		virtual VkDescriptorSetLayoutBinding GetDescriptorLayout() const = 0;
		virtual VkWriteDescriptorSet GetDescriptorWrite(VkDescriptorSet _descriptorSet) const = 0;
	};
}