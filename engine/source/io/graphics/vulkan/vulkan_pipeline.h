#pragma once
#include <vulkan/vulkan.h>
#include "io/graphics/graphics_api.h"
#include "utility/stl.h"

namespace io::graphics
{
	class VulkanDevice;
	class VulkanSwapChain;
	struct PipelineState;

	class VulkanPipeline : public Pipeline
	{
	private:
		const VulkanDevice& device_;

		VkPipeline instance_{};

		VkBuffer vertexBuffer_{};
		VkBuffer indexBuffer_{};
		VkDeviceMemory vertexBufferMemory_{};
		VkDeviceMemory indexBufferMemory_{};

	public:
		VulkanPipeline(const VulkanDevice& _device, const PipelineState& _pipelineState, const VulkanSwapChain& _swapChain);
		~VulkanPipeline();

	public:
		VkPipeline GetInstance() const;
		VkBuffer GetVertexBuffer() const;
	};
}