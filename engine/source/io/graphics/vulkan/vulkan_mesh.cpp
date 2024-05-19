#include "vulkan_mesh.h"
#include "vulkan_validation.hpp"
#include "vulkan_utility.h"

namespace io::graphics
{
	void CreateBuffer(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _properties,  uint32_t _bufferSize, VkBuffer& _outBuffer, VkDeviceMemory& _outBufferMemory)
	{
		VkBufferCreateInfo bufferCreateInfo{};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.size = _bufferSize;
		bufferCreateInfo.usage = _usage;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		vkCreateBuffer(_logicalDevice, &bufferCreateInfo, nullptr, &_outBuffer) >> VulkanResultChecker::GetInstance();

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(_logicalDevice, _outBuffer, &memRequirements);

		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &memProperties);

		// find memory type
		uint32_t memoryTypeIndex = std::numeric_limits<uint32_t>::max();
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((memRequirements.memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & _properties) == _properties)
			{
				memoryTypeIndex = i;
				break;
			}
		}

		if (memoryTypeIndex == std::numeric_limits<uint32_t>::max())
		{
			throw std::exception("CreateBuffer() : failed to find required memory type");
		}

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = memoryTypeIndex;

		vkAllocateMemory(_logicalDevice, &allocInfo, nullptr, &_outBufferMemory) >> VulkanResultChecker::GetInstance();
		vkBindBufferMemory(_logicalDevice, _outBuffer, _outBufferMemory, 0) >> VulkanResultChecker::GetInstance();
	}

	void CopyBuffer(VkDevice _logicalDevice, VkQueue _transferQueue, VkCommandPool _commandPool, VkBuffer _srcBuffer, VkBuffer _dstBuffer, uint32_t _bufferSize)
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = _commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(_logicalDevice, &allocInfo, &commandBuffer) >> VulkanResultChecker::GetInstance();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = _bufferSize;
		vkCmdCopyBuffer(commandBuffer, _srcBuffer, _dstBuffer, 1, &copyRegion);
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(_transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(_transferQueue);

		vkFreeCommandBuffers(_logicalDevice, _commandPool, 1, &commandBuffer);
	}

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