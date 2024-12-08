#pragma once
#include "graphics/graphics_api.h"
#include "utility/forward_declaration.h"
#include <vulkan/vulkan.h>

namespace graphics
{
	class VulkanPipeline : public Pipeline
	{
	private:
		VkDevice logicalDevice_;
		VkPhysicalDevice physicalDevice_;
		VkPipeline instance_;
		VkShaderModule vertexShaderModule_;
		VkShaderModule pixelShaderModule_;
		VkRenderPass renderPass_;
		VkPipelineLayout layout_;
		const bool useDepthStencil_;

		VkDescriptorSetLayout materialDescriptorSetLayout_;
		VkDescriptorSetLayout descriptorSetLayout_;

		bool pendingDescriptorSetUpdate_ = false;

	public:
		VulkanPipeline(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, const Pipeline::Layout& _pipelineLayout);
		~VulkanPipeline();

	public:
		virtual std::shared_ptr<RenderTarget> CreateRenderTarget(uint32_t _width, uint32_t _height) const override;
		virtual bool BindShaderBinding(std::shared_ptr<ShaderBinding> _shaderBinding, uint32_t _slot) override;

		bool IsPendingDescriptorSetUpdate() const;
		void UpdateDescriptorSet(const std::vector<VkDescriptorSet>& _descriptSets);

		VkPipeline GetInstance() const;
		VkPipelineLayout GetLayout() const;
		VkRenderPass GetRenderPass() const;

		VkDescriptorSetLayout GetDescriptorSetLayout() const;
		uint32_t GetNumBindings() const;

	private:
		void LoadShaders(std::wstring_view _vsPath, std::wstring_view _fsPath);
		void CreateInstance(VkPhysicalDevice _physicalDevice, const Pipeline::Layout& _pipelineLayout);
	};
}