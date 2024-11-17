#include "vulkan_material.h"

namespace graphics
{
	VkDescriptorSetLayout VulkanMaterial::descriptorSetLayout_;

	VulkanMaterial::VulkanMaterial(Initializer _initializer, const Material::Layout& _layout)
		: logicalDevice_(_initializer.logicalDevice_)
	{
		if (descriptorSetLayout_ == VK_NULL_HANDLE)
		{
			CreateDescriptorSetLayout();
		}

		CreateDescriptorSet(_initializer.descriptorPool_);
	}

	VkDescriptorSetLayout VulkanMaterial::GetDescriptorSetLayout() const
	{
		return descriptorSetLayout_;
	}

	void VulkanMaterial::CreateDescriptorSetLayout()
	{
		std::vector<VkDescriptorSetLayoutBinding> bindings;
		{
			VkDescriptorSetLayoutBinding diffuseMapBinding{};
			diffuseMapBinding.binding = reservedBindingOffset - (uint8_t)ReservedBindings::DIFFUSE_MAP;
			diffuseMapBinding.descriptorCount = 1;
			diffuseMapBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			diffuseMapBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			//diffuseMapBinding.pImmutableSamplers = nullptr;
			bindings.push_back(diffuseMapBinding);
		}
		{
			VkDescriptorSetLayoutBinding normalMapBinding{};
			normalMapBinding.binding = reservedBindingOffset - (uint8_t)ReservedBindings::NORMAL_MAP;
			normalMapBinding.descriptorCount = 1;
			normalMapBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			normalMapBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			//normalMapBinding.pImmutableSamplers = nullptr;
			bindings.push_back(normalMapBinding);
		}

		VkDescriptorSetLayoutCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		createInfo.bindingCount = (uint32_t)bindings.size();
		createInfo.pBindings = bindings.data();
		//createInfo.flags = NULL;
		//createInfo.pNext = nullptr;

		vkCreateDescriptorSetLayout(logicalDevice_, &createInfo, nullptr, &descriptorSetLayout_);
	}

	void VulkanMaterial::CreateDescriptorSet(VkDescriptorPool _descriptorPool)
	{
		VkDescriptorSetAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocateInfo.descriptorPool = _descriptorPool;
		allocateInfo.descriptorSetCount = 1;
		allocateInfo.pSetLayouts = &descriptorSetLayout_;

		vkAllocateDescriptorSets(logicalDevice_, &allocateInfo, &descriptorSet_);
	}
}