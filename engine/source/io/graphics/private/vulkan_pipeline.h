#pragma once
#include <vulkan/vulkan.h>
#include "utility/stl.h"

namespace io::graphics
{
	class VulkanDevice;
	class VulkanCommander;
	class VulkanSwapChain;
	struct PipelineState;

	class VulkanPipeline
	{
	private:
		VkPipeline instance_;

		VkBuffer vertexBuffer_;
		VkBuffer indexBuffer_;
		VkDeviceMemory vertexBufferMemory_;
		VkDeviceMemory indexBufferMemory_;

	public:
		void Initialize(const VulkanDevice& _device, const PipelineState& _pipelineState, const VulkanCommander& _commander, const VulkanSwapChain& _swapChain);
		void Release(const VulkanDevice& _device);

		VkPipeline GetInstance() const;
		VkBuffer GetVertexBuffer() const;

	private:
		VkPipelineLayout CreatePipelineLayout(const VulkanDevice& _device);
		std::vector<VkPipelineShaderStageCreateInfo> CreateShaderStageInfos(const VulkanDevice& _device, const PipelineState& _pipelineState);
		VkVertexInputBindingDescription CreateVertexBindingDesc(const PipelineState& _pipelineState);
		std::vector<VkVertexInputAttributeDescription> CreateVertexAttributeDescs(const PipelineState& _pipelineState);
		VkPipelineVertexInputStateCreateInfo CreateVertexInputInfo(const PipelineState& _pipelineState, VkVertexInputBindingDescription& _bindingDescription, std::vector<VkVertexInputAttributeDescription>& _attibuteDescs);
		void CreateVertexBuffer(const VulkanDevice& _device, const PipelineState& _pipelineState, const VulkanCommander& _commander);
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