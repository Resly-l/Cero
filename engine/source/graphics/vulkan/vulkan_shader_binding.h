#pragma once
#include <vulkan/vulkan.h>
#include "graphics/shader.h"

namespace graphics
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