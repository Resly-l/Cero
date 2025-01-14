#include "vulkan_utility.h"
#include "vulkan_result.hpp"

namespace graphics
{
	uint32_t FindMemoryTypeIndex(VkPhysicalDevice _physicalDevice, VkMemoryRequirements _memoryRequirements, VkMemoryPropertyFlags _requiredProperties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((_memoryRequirements.memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & _requiredProperties) == _requiredProperties)
			{
				return i;
			}
		}

		return std::numeric_limits<uint32_t>::max();
	}

	void CreateBuffer(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _properties, VkDeviceSize _bufferSize, VkBuffer& _outBuffer, VkDeviceMemory& _outBufferMemory)
	{
		VkBufferCreateInfo bufferCreateInfo{};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.size = _bufferSize;
		bufferCreateInfo.usage = _usage;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		vkCreateBuffer(_logicalDevice, &bufferCreateInfo, nullptr, &_outBuffer) >> VulkanResultChecker::Get();

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(_logicalDevice, _outBuffer, &memRequirements);

		uint32_t memoryTypeIndex = FindMemoryTypeIndex(_physicalDevice, memRequirements, _properties);
		if (memoryTypeIndex == std::numeric_limits<uint32_t>::max())
		{
			throw std::exception("CreateBuffer() : failed to find required memory type");
		}

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = memoryTypeIndex;

		vkAllocateMemory(_logicalDevice, &allocInfo, nullptr, &_outBufferMemory) >> VulkanResultChecker::Get();
		vkBindBufferMemory(_logicalDevice, _outBuffer, _outBufferMemory, 0) >> VulkanResultChecker::Get();
	}

	void CopyBuffer(VkDevice _logicalDevice, VkQueue _transferQueue, VkCommandPool _commandPool, VkBuffer _srcBuffer, VkBuffer _dstBuffer, VkDeviceSize _bufferSize)
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = _commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(_logicalDevice, &allocInfo, &commandBuffer) >> VulkanResultChecker::Get();

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

	VkPrimitiveTopology VulkanTypeConverter::Convert(PrimitiveTopology _topology)
	{
		switch (_topology)
		{
		case graphics::PrimitiveTopology::NONE:
			throw std::runtime_error("primitive topology is not set");
			break;
		case graphics::PrimitiveTopology::LINE_LIST:
			return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		case graphics::PrimitiveTopology::TRIANGLE_LIST:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		}

		return VkPrimitiveTopology{};
	}

	VkFormat VulkanTypeConverter::Convert(ImageFormat _format)
	{
		switch (_format)
		{
		case ImageFormat::R8G8B8_UNORM:
			return VK_FORMAT_R8G8B8_UNORM;
		case ImageFormat::R8G8B8_NORM:
			return VK_FORMAT_R8G8B8_SNORM;
		case ImageFormat::R8G8B8A8_UNORM:
			return VK_FORMAT_R8G8B8A8_UNORM;
		case ImageFormat::R8G8B8A8_NORM:
			return VK_FORMAT_R8G8B8A8_SNORM;
		case ImageFormat::B8G8R8_UNORM:
			return VK_FORMAT_B8G8R8_UNORM;
		case ImageFormat::B8G8R8_NORM:
			return VK_FORMAT_B8G8R8_SNORM;
		case ImageFormat::B8G8R8A8_UNORM:
			return VK_FORMAT_B8G8R8A8_UNORM;
		case ImageFormat::B8G8R8A8_NORM:
			return VK_FORMAT_B8G8R8A8_SNORM;
			case ImageFormat::D32_SFLOAT:
			return VK_FORMAT_D32_SFLOAT;
		case ImageFormat::D32_SFLOAT_U8_UINT:
			return VK_FORMAT_D32_SFLOAT_S8_UINT;
		}

		return VK_FORMAT_UNDEFINED;
	}

	VkImageUsageFlags VulkanTypeConverter::Convert(ImageUsage _usage)
	{
		switch (_usage)
		{
		case ImageUsage::COLOR_ATTACHMENT:
			return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		case ImageUsage::DEPTH_STENCIL:
			return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		}

		return VkImageUsageFlags{};
	}

	VkAttachmentLoadOp VulkanTypeConverter::ConvertLoadOp(ImageOperation _operation)
	{
		switch (_operation)
		{
		case ImageOperation::STORE:
			return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_LOAD;
		case ImageOperation::CLEAR:
			return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
		}

		return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	}

    VkAttachmentStoreOp VulkanTypeConverter::ConvertStoreOp(ImageOperation _operation)
    {
        switch (_operation)
		{
		case ImageOperation::STORE:
			return VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
		case ImageOperation::CLEAR:
			return VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
		}

		return VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
    }

	VkDescriptorType VulkanTypeConverter::Convert(ShaderBinding::Type _type)
	{
		switch (_type)
		{
		case ShaderBinding::Type::UNIFORM_BUFFER:
			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		case ShaderBinding::Type::TEXTURE_2D:
			return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		}
		
		return VkDescriptorType{};
	}

	VkShaderStageFlags VulkanTypeConverter::Convert(ShaderStage _stage)
	{
		VkShaderStageFlags flags{};
		if ((uint8_t)_stage & (uint8_t)ShaderStage::VERTEX)
		{
			flags |= VK_SHADER_STAGE_VERTEX_BIT;
		}
		if ((uint8_t)_stage & (uint8_t)ShaderStage::PIXEL)
		{
			flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
		}

		return flags;
	}
}