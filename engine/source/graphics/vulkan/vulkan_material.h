#pragma once
#include "graphics/graphics_api.h"
#include <vulkan/vulkan.h>

namespace graphics
{
	class VulkanMaterial : public Material
	{
	public:
		struct Initializer
		{
			VkDevice logicalDevice_;
			VkDescriptorPool descriptorPool_;
		};

	private:
		static VkDescriptorSetLayout descriptorSetLayout_;
		VkDevice logicalDevice_;
		VkDescriptorSet descriptorSet_;

	public:
		VulkanMaterial(Initializer _initializer, const Material::Layout& _layout);

	public:
		VkDescriptorSetLayout GetDescriptorSetLayout() const;

	private:
		void CreateDescriptorSetLayout();
		void CreateDescriptorSet(VkDescriptorPool _descriptorPool);
	};
}