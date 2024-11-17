#pragma once
#include "graphics/graphics_api.h"
#include <vulkan/vulkan.h>

namespace graphics
{
	class VulkanModel : public Model
	{
	public:
		struct Initializer
		{
			VkDevice logicalDevice_;
			VkPhysicalDevice physicalDevice_;
			VkQueue tranferQueue_;
			VkCommandPool transferCommandPool_;
			VkDescriptorPool descriptorPool_;
		};

	private:


	public:
		VulkanModel(Initializer _initializer, const Model::Layout& _modelLayout);

	public:

	};
}