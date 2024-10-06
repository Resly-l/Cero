#pragma once
#include <vulkan/vulkan.h>
#include "graphics/shader.h"

namespace graphics
{
	class VulkanShaderBindingImpl : public ShaderBinding::ApiSpecificImpl
	{
	public:
		uint32_t slot_ = 0;
		uint32_t numElements_ = 1;
		VkShaderStageFlags stage_{};

	public:
		virtual ~VulkanShaderBindingImpl() {}

	public:
		virtual VkWriteDescriptorSet GetDescriptorWrite(VkDescriptorSet _descriptorSet) const = 0;
	};
}