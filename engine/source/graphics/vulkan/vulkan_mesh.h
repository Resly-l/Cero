#pragma once
#include "graphics/graphics_api.h"
#include <vulkan/vulkan.h>

namespace graphics
{
	class VulkanMesh : public Mesh
	{
	private:
		VkDevice logicalDevice_;
		VkBuffer vertexBuffer_;
		VkBuffer indexBuffer_;
		VkDeviceMemory vertexBufferMemory_;
		VkDeviceMemory indexBufferMemory_;
		
	public:
		VulkanMesh(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, VkQueue _transferQueue, VkCommandPool _transferCommandPool,  const Mesh::Layout& _meshLayout);
		~VulkanMesh();

	public:
		VkBuffer GetVertexBuffer() const;
		VkBuffer GetIndexBuffer() const;

	private:
		void CreateVertexBuffer(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, VkQueue _transferQueue, VkCommandPool _transferCommandPool,  const utility::ByteBuffer& _vertices);
		void CreateIndexBuffer(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, VkQueue _transferQueue, VkCommandPool _transferCommandPool, const std::vector<uint32_t>& _indices);
	};
}