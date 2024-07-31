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
		VkPhysicalDevice physicalDevice_;
		VkPipeline instance_;
		VkShaderModule vertexShaderModule_;
		VkShaderModule pixelShaderModule_;
		VkRenderPass renderPass_;
		ShaderDescriptor shaderDescriptor_;
		VkPipelineLayout layout_;
		const bool useDepthStencil_;

		std::vector<std::unique_ptr<VulkanUniformBuffer>> uniformBuffers_;
		VkDescriptorPool descriptorPool_;
		VkDescriptorSetLayout descriptorSetLayout_;

	public:
		VulkanPipeline(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, const Pipeline::Layout& _pipelineLayout, VkDescriptorPool _descriptorPool);
		~VulkanPipeline();

	public:
		virtual std::shared_ptr<RenderTarget> CreateRenderTarget(uint32_t _width, uint32_t _height) const override;

		VkPipeline GetInstance() const;
		VkPipelineLayout GetLayout() const;
		VkRenderPass GetRenderPass() const;
		VkDescriptorSetLayout GetDescriptorSetLayout() const;
		uint32_t GetNumBindings() const;

		void UpdateDescriptorSet(VkDescriptorSet _descriptorSet);
		virtual void UpdateUniformBuffer(uint32_t _index, const utility::ByteBuffer& _buffer) override;

	private:
		void LoadShaders(std::wstring_view _vsPath, std::wstring_view _fsPath);
		void CreateInstance(VkPhysicalDevice _physicalDevice, const Pipeline::Layout& _pipelineLayout);
	};
}