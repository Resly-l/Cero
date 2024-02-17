#pragma once
#include "io/graphics/graphics_api.h"
#include "vulkan/vulkan.h"

namespace io::graphics
{
	class VulkanPipeline : public Pipeline
	{
	private:
		const VkDevice& logicalDevice_;
		VkPipeline instance_;
		VkBuffer vertexBuffer_;
		VkDeviceMemory vertexBufferMemory_;
		VkBuffer indexBuffer_;
		VkDeviceMemory indexBufferMemory_;

	public:
		VulkanPipeline(const VkDevice& _logicalDevice, VkPhysicalDevice _physicalDevice, VkCommandPool _commandPool, VkQueue _queue, const PipelineState& _pipelineState, VkRenderPass _renderPass, uint32_t _numFramebufferAttachements);
		~VulkanPipeline();

	public:
		VkPipeline GetInstance() const;
		VkBuffer GetVertexBuffer() const;
		VkBuffer GetIndexBuffer() const;
	};
}