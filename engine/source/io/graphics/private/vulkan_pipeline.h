#pragma once
#include <vulkan/vulkan.h>
#include "io/graphics/graphics_api.h"
#include "utility/stl.h"

namespace io::graphics
{
	class VulkanDevice;
	class VulkanCommander;
	class VulkanSwapChain;
	struct PipelineState;

	class VulkanPipeline : public Pipeline
	{
	private:
		const VulkanDevice& device_;

		VkPipeline instance_;

		VkBuffer vertexBuffer_;
		VkBuffer indexBuffer_;
		VkDeviceMemory vertexBufferMemory_;
		VkDeviceMemory indexBufferMemory_;

	public:
		VulkanPipeline(const VulkanDevice& _device, const PipelineState& _pipelineState, const VulkanCommander& _commander, const VulkanSwapChain& _swapChain);
		~VulkanPipeline();

		VkPipeline GetInstance() const;
		VkBuffer GetVertexBuffer() const;

	private:
		VkPipelineLayout CreatePipelineLayout();
		std::vector<VkPipelineShaderStageCreateInfo> CreateShaderStageInfos(const PipelineState& _pipelineState);
		VkVertexInputBindingDescription CreateVertexBindingDesc(const PipelineState& _pipelineState);
		std::vector<VkVertexInputAttributeDescription> CreateVertexAttributeDescs(const PipelineState& _pipelineState);
		VkPipelineVertexInputStateCreateInfo CreateVertexInputInfo(const PipelineState& _pipelineState, VkVertexInputBindingDescription& _bindingDescription, std::vector<VkVertexInputAttributeDescription>& _attibuteDescs);
		void CreateVertexBuffer(const PipelineState& _pipelineState, const VulkanCommander& _commander);
		VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyInfo(const PipelineState& _pipelineState);
		VkViewport CreateViewport(const PipelineState& _pipelineState);
		VkRect2D CreateScissor(const PipelineState& _pipelineState);
		VkPipelineViewportStateCreateInfo CreateViewportInfo(const PipelineState& _pipelineState, VkViewport& _viewport, VkRect2D& scissor_);
		VkPipelineRasterizationStateCreateInfo CreateRasterizationInfo(const PipelineState& _pipelineState);
		VkPipelineMultisampleStateCreateInfo CreateMultisampleInfo(const PipelineState& _pipelineState);
		VkPipelineDepthStencilStateCreateInfo CreateDepthStencilInfo(const PipelineState& _pipelineState);
		VkPipelineColorBlendAttachmentState CreateColorBlendingAttachmentInfo(const PipelineState& _pipelineState);
		VkPipelineColorBlendStateCreateInfo CreateColorBlendInfo(const PipelineState& _pipelineState, VkPipelineColorBlendAttachmentState& _colorBlendAttachment);
	};
}