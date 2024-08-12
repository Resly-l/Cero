#include "vulkan_mesh.h"
#include "vulkan_result.hpp"
#include "vulkan_utility.h"

namespace io::graphics
{
	VulkanMesh::VulkanMesh(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, VkQueue _transferQueue, VkCommandPool _transferCommandPool, const Mesh::Layout& _meshLayout)
		: logicalDevice_(_logicalDevice)
		, numVertices_((uint32_t)_meshLayout.vertices_.GetNumElements())
		, numIndices_((uint32_t)_meshLayout.indices_.size())
	{
		CreateVertexBuffer(logicalDevice_, _physicalDevice, _transferQueue, _transferCommandPool, _meshLayout.vertices_);
		CreateIndexBuffer(logicalDevice_, _physicalDevice, _transferQueue, _transferCommandPool, _meshLayout.indices_);
	}

	VulkanMesh::~VulkanMesh()
	{
		vkDestroyBuffer(logicalDevice_, indexBuffer_, nullptr);
		vkFreeMemory(logicalDevice_, indexBufferMemory_, nullptr);

		vkDestroyBuffer(logicalDevice_, vertexBuffer_, nullptr);
		vkFreeMemory(logicalDevice_, vertexBufferMemory_, nullptr);
	}

	VkBuffer VulkanMesh::GetVertexBuffer() const
	{
		return vertexBuffer_;
	}

	VkBuffer VulkanMesh::GetIndexBuffer() const
	{
		return indexBuffer_;
	}

	uint32_t VulkanMesh::GetNumVertices() const
	{
		return numVertices_;
	}

	uint32_t VulkanMesh::GetNumIndices() const
	{
		return numIndices_;
	}

	void VulkanMesh::CreateVertexBuffer(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, VkQueue _transferQueue, VkCommandPool _transferCommandPool,  const utility::ByteBuffer& _vertices)
	{
		uint32_t vertexBufferSize = (uint32_t)_vertices.GetSizeInBytes();

		VkBuffer vertexStagingBuffer;
		VkDeviceMemory vertexStagingBufferMemory;
		VkBufferUsageFlags vertexStagingUsageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		VkMemoryPropertyFlags stagingMemoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		CreateBuffer(logicalDevice_, _physicalDevice, vertexStagingUsageFlags, stagingMemoryProperties, vertexBufferSize, vertexStagingBuffer, vertexStagingBufferMemory);

		void* data = nullptr;
		vkMapMemory(logicalDevice_, vertexStagingBufferMemory, 0, vertexBufferSize, 0, &data) >> VulkanResultChecker::GetInstance();
		memcpy(data, _vertices.GetRawBufferAddress(), vertexBufferSize);
		vkUnmapMemory(logicalDevice_, vertexStagingBufferMemory);

		VkBufferUsageFlags vertexBufferUsageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		VkMemoryPropertyFlags vertexBufferMemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		CreateBuffer(logicalDevice_, _physicalDevice, vertexBufferUsageFlags, vertexBufferMemoryProperties, vertexBufferSize, vertexBuffer_, vertexBufferMemory_);

		CopyBuffer(logicalDevice_, _transferQueue, _transferCommandPool, vertexStagingBuffer, vertexBuffer_, vertexBufferSize);

		vkDestroyBuffer(logicalDevice_, vertexStagingBuffer, nullptr);
		vkFreeMemory(logicalDevice_, vertexStagingBufferMemory, nullptr);
	}

	void VulkanMesh::CreateIndexBuffer(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, VkQueue _transferQueue, VkCommandPool _transferCommandPool, const std::vector<uint32_t>& _indices)
	{
		uint32_t indexBufferSize = (uint32_t)_indices.size() * sizeof(uint32_t);

		VkBuffer indexStagingBuffer;
		VkDeviceMemory indexStagingBufferMemory;
		VkBufferUsageFlags vertexStagingUsageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		VkMemoryPropertyFlags stagingMemoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		CreateBuffer(logicalDevice_, _physicalDevice, vertexStagingUsageFlags, stagingMemoryProperties, indexBufferSize, indexStagingBuffer, indexStagingBufferMemory);

		void* data = nullptr;
		vkMapMemory(logicalDevice_, indexStagingBufferMemory, 0, indexBufferSize, 0, &data) >> VulkanResultChecker::GetInstance();
		memcpy(data, _indices.data(), indexBufferSize);
		vkUnmapMemory(logicalDevice_, indexStagingBufferMemory);

		VkBufferUsageFlags vertexBufferUsageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		VkMemoryPropertyFlags vertexBufferMemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		CreateBuffer(logicalDevice_, _physicalDevice, vertexBufferUsageFlags, vertexBufferMemoryProperties, indexBufferSize, indexBuffer_, indexBufferMemory_);

		CopyBuffer(logicalDevice_, _transferQueue, _transferCommandPool, indexStagingBuffer, indexBuffer_, indexBufferSize);

		vkDestroyBuffer(logicalDevice_, indexStagingBuffer, nullptr);
		vkFreeMemory(logicalDevice_, indexStagingBufferMemory, nullptr);
	}
}