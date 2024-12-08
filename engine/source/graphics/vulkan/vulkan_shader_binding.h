#pragma once
#include "graphics/shader.h"
#include "vulkan/vulkan.h"

namespace graphics
{
	class VulkanShaderBinding : public ShaderBinding::BindingImpl
	{
	public:
		virtual void FillBindingInfo(VkWriteDescriptorSet& _WriteDescriptorSet) const = 0;
	};
}