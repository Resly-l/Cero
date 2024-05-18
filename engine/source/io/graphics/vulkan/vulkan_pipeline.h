#pragma once
#include "io/graphics/graphics_api.h"
#include "utility/forward_declaration.h"
#include <vulkan/vulkan.h>

namespace io::graphics
{
	class VulkanPipeline : public Pipeline
	{
	private:
		VkDevice logicalDevice_;
		VkPipeline instance_;
		VkRenderPass renderPass_;
		VkShaderModule vertexShaderModule_;
		VkShaderModule pixelShaderModule_;

	public:
		VulkanPipeline(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, const Pipeline::Layout& _pipelineLayout);
		~VulkanPipeline();

	public:
		VkPipeline GetInstance() const;
		VkRenderPass GetRenderPass() const;

	private:
		void LoadShaders(std::wstring_view _vsPath, std::wstring_view _fsPath);
		void CreateInstance(VkPhysicalDevice _physicalDevice, const Pipeline::Layout& _pipelineLayout);
	};
}