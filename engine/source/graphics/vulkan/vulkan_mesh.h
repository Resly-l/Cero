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
		uint32_t numVertices_;
		uint32_t numIndices_;
		
	public:
		VulkanMesh(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, VkQueue _transferQueue, VkCommandPool _transferCommandPool,  const Mesh::Layout& _meshLayout);
		~VulkanMesh();

	public:
		VkBuffer GetVertexBuffer() const;
		VkBuffer GetIndexBuffer() const;
		virtual uint32_t GetNumVertices() const override;
		virtual uint32_t GetNumIndices() const override;

	private:
		void CreateVertexBuffer(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, VkQueue _transferQueue, VkCommandPool _transferCommandPool,  const utility::ByteBuffer& _vertices);
		void CreateIndexBuffer(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, VkQueue _transferQueue, VkCommandPool _transferCommandPool, const std::vector<uint32_t>& _indices);
	};
}