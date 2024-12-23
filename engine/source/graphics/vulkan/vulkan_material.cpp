#include "vulkan_material.h"
#include "vulkan_utility.h"
#include "vulkan_shader_binding.h"
#include "utility/log.h"

namespace graphics
{
	VkDescriptorSetLayout VulkanMaterial::descriptorSetLayout_;

	VulkanMaterial::VulkanMaterial(Initializer _initializer)
		: logicalDevice_(_initializer.logicalDevice_)
		, descriptorPool_(_initializer.descriptorPool_)
	{
		if (descriptorSetLayout_ == VK_NULL_HANDLE)
		{
			descriptorSetLayout_ = CreateDescriptorSetLayout(logicalDevice_);
		}

		CreateDescriptorSet(_initializer.descriptorPool_);
	}

	VulkanMaterial::~VulkanMaterial()
	{
		if (descriptorSetLayout_ != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorSetLayout(logicalDevice_, descriptorSetLayout_, nullptr);
			descriptorSetLayout_ = VK_NULL_HANDLE;
		}

		vkDestroyDescriptorPool(logicalDevice_, descriptorPool_, nullptr);
	}

	std::vector<VkDescriptorSetLayoutBinding> VulkanMaterial::GetDescriptorSetLayoutBindings()
	{
		static std::vector<VkDescriptorSetLayoutBinding> bindings;

		if (bindings.empty())
		{
			uint32_t binding = 0;
			{
				VkDescriptorSetLayoutBinding diffuseMapBinding{};
				diffuseMapBinding.binding = binding++;
				diffuseMapBinding.descriptorCount = 1;
				diffuseMapBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				diffuseMapBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				//diffuseMapBinding.pImmutableSamplers = nullptr;
				bindings.push_back(diffuseMapBinding);
			}
			{
				VkDescriptorSetLayoutBinding normalMapBinding{};
				normalMapBinding.binding = binding++;
				normalMapBinding.descriptorCount = 1;
				normalMapBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				normalMapBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				//normalMapBinding.pImmutableSamplers = nullptr;
				bindings.push_back(normalMapBinding);
			}
		}

		return bindings;
	}

	VkDescriptorSetLayout VulkanMaterial::CreateDescriptorSetLayout(VkDevice _logicalDevice)
	{
		VkDescriptorSetLayout descriptorSetLayout;
		std::vector<VkDescriptorSetLayoutBinding> bindings = GetDescriptorSetLayoutBindings();

		VkDescriptorSetLayoutCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		createInfo.bindingCount = (uint32_t)bindings.size();
		createInfo.pBindings = bindings.data();
		//createInfo.flags = NULL;

		vkCreateDescriptorSetLayout(_logicalDevice, &createInfo, nullptr, &descriptorSetLayout);
		return descriptorSetLayout;
	}

	VkDescriptorSet VulkanMaterial::GetDescriptorSet() const
	{
		return descriptorSet_;
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

	void VulkanMaterial::UpdateDescriptorSet()
	{
		if (pendingCompilation_ == false)
		{
			using utility::Log;
			std::cout << Log::Format(Log::Category::graphics, Log::Level::message, "Material - Tried to update descriptor set when there's no change") << std::endl;
			return;
		}

		std::vector<VkWriteDescriptorSet> descriptorWrites;
		for (uint32_t i = 0; i < fixedBindings_.size(); i++)
		{
			VkWriteDescriptorSet write{};
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.pNext = nullptr;
			write.dstSet = descriptorSet_;
			write.dstBinding = i;
			write.dstArrayElement = 0;
			write.descriptorCount = 1;
			write.pTexelBufferView = nullptr;

			auto casted = std::static_pointer_cast<VulkanShaderBinding>(fixedBindings_[i]->GetBindingImpl());
			casted->FillBindingInfo(write);

			descriptorWrites.push_back(write);
		}

		vkUpdateDescriptorSets(logicalDevice_, (uint32_t)descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
		pendingCompilation_ = false;
	}
}