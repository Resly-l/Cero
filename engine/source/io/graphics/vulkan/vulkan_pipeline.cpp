#include "vulkan_pipeline.h"
#include "vulkan_bootstrap.h"

namespace io::graphics
{
	VulkanPipeline::VulkanPipeline(const VkDevice& _logicalDevice, VkPhysicalDevice _physicalDevice, VkCommandPool _commandPool, VkQueue _queue, const PipelineState& _pipelineState, VkRenderPass _renderPass, uint32_t _numFramebufferAttachements)
		: logicalDevice_(_logicalDevice)
	{
		instance_ = VulkanBootstrap::CreatePipeline(logicalDevice_, _pipelineState, _renderPass, _numFramebufferAttachements);

		auto [vertexBuffer, vertexBufferMemory] = VulkanBootstrap::CreateVertexBuffer(logicalDevice_, _physicalDevice, _commandPool, _queue, _pipelineState.vertexBuffer.GetRawBufferAddress(), _pipelineState.vertexBuffer.GetSizeInBytes());
		vertexBuffer_ = vertexBuffer;
		vertexBufferMemory_ = vertexBufferMemory;
	}

	VulkanPipeline::~VulkanPipeline()
	{
		vkFreeMemory(logicalDevice_, vertexBufferMemory_, nullptr);
		//vkFreeMemory(logicalDevice_, indexBufferMemory_, nullptr);
		vkDestroyBuffer(logicalDevice_, vertexBuffer_, nullptr);
		//vkDestroyBuffer(logicalDevice_, indexBuffer_, nullptr);
		vkDestroyPipeline(logicalDevice_, instance_, nullptr);
	}

	VkPipeline VulkanPipeline::GetInstance() const
	{
		return instance_;
	}

	VkBuffer VulkanPipeline::GetVertexBuffer() const
	{
		return vertexBuffer_;
	}

	VkBuffer VulkanPipeline::GetIndexBuffer() const
	{
		return indexBuffer_;
	}
}