#pragma once
#include "graphics/material.h"
#include <vector>
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
		VkDescriptorPool descriptorPool_;

	public:
		VulkanMaterial(Initializer _initializer);
		~VulkanMaterial();

	public:
		static std::vector< VkDescriptorSetLayoutBinding> GetDescriptorSetLayoutBindings();
		static VkDescriptorSetLayout CreateDescriptorSetLayout(VkDevice _logicalDevice);
		VkDescriptorSet GetDescriptorSet() const;
		void UpdateDescriptorSet();

	private:
		void CreateDescriptorSet(VkDescriptorPool _descriptorPool);
	};
}