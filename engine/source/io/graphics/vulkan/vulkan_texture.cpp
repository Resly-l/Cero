#include "vulkan_texture.h"
#include "vulkan_buffer.h"
#include "vulkan_result.hpp"
#include "vulkan_utility.h"

namespace io::graphics
{
	VulkanTexture::VulkanTexture(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, VkQueue _graphicsQueue, VkCommandPool _commandPool, const Texture::Layout& _layout)
		: logicalDevice_(_logicalDevice)
	{
		slot_ = _layout.slot_;
		numElements_ = _layout.numElements_;
		stage_ = VulkanTypeConverter::Convert(_layout.stage_);
		format_ = VulkanTypeConverter::Convert(_layout.format_);

		file::Image image = LoadImage(_layout.imagePath_);
		VkBuffer stagingBuffer = VK_NULL_HANDLE;
		VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

		CreateStagingBuffer(_physicalDevice, image, stagingBuffer, stagingBufferMemory);
		CreateImage(_physicalDevice);
		{
			TransitImageLayout(format_, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, _graphicsQueue, _commandPool);
			CopyBufferToImage(stagingBuffer, _graphicsQueue, _commandPool);
			TransitImageLayout(format_, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, _graphicsQueue, _commandPool);
		}
		CreateImageView();
		CreateSampler(_physicalDevice);

		vkFreeMemory(logicalDevice_, stagingBufferMemory, nullptr);
		vkDestroyBuffer(logicalDevice_, stagingBuffer, nullptr);

		imageInfo_.imageView = imageView_;
		imageInfo_.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo_.sampler = sampler_;
	}

	VulkanTexture::~VulkanTexture()
	{
		vkDestroySampler(logicalDevice_, sampler_, nullptr);
		vkFreeMemory(logicalDevice_, imageMemory_, nullptr);
		vkDestroyImageView(logicalDevice_, imageView_, nullptr);
		vkDestroyImage(logicalDevice_, image_, nullptr);
	}

	uint32_t VulkanTexture::GetWidth() const
	{
		return width_;
	}

	uint32_t VulkanTexture::GetHeight() const
	{
		return height_;
	}

	class VulkanTextureBinding : public VulkanShaderBinding
	{
	public:
		uint32_t slot_ = 0;
		uint32_t numElements_ = 1;
		VkShaderStageFlags stage_{};
		VkDescriptorImageInfo imageInfo_{};

	public:
		virtual VkDescriptorSetLayoutBinding GetDescriptorLayout() const override
		{
			VkDescriptorSetLayoutBinding layout{};
			layout.binding = slot_;
			layout.descriptorCount = numElements_;
			layout.pImmutableSamplers = nullptr;
			layout.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			layout.stageFlags = stage_;
			return layout;
		}

		virtual VkWriteDescriptorSet GetDescriptorWrite(VkDescriptorSet _descriptorSet) const override
		{
			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = _descriptorSet;
			descriptorWrite.dstBinding = slot_;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pImageInfo = &imageInfo_;
			return descriptorWrite;
		}
	};

    std::shared_ptr<ShaderBinding> VulkanTexture::GetShaderBinding() const
    {
		auto binding = std::make_shared<VulkanTextureBinding>();
		binding->slot_ = slot_;
		binding->numElements_ = numElements_;
		binding->stage_ = stage_;
		binding->imageInfo_ = imageInfo_;

		return binding;
    }

	file::Image VulkanTexture::LoadImage(std::string_view _path)
	{
		file::Image image;
		image.Load(_path);
		if (!image.IsLoaded())
		{
			throw std::runtime_error(std::string("failed to load image") + std::string(_path));
		}

		width_ = image.width_;
		height_ = image.height_;

		return image;
	}

	void VulkanTexture::CreateStagingBuffer(VkPhysicalDevice _physicalDevice, const file::Image& _image, VkBuffer& _outStagingBuffer, VkDeviceMemory& _outBufferMemory)
	{
		VkBufferUsageFlags stagingBufferUsages = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		VkMemoryPropertyFlags stagingBufferProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		CreateBuffer(logicalDevice_, _physicalDevice, stagingBufferUsages, stagingBufferProperties, _image.colors_.size(), _outStagingBuffer, _outBufferMemory);

		void* data = nullptr;
		vkMapMemory(logicalDevice_, _outBufferMemory, 0, _image.colors_.size(), 0, &data);
		memcpy(data, _image.colors_.data(), _image.colors_.size());
		vkUnmapMemory(logicalDevice_, _outBufferMemory);
	}

	void VulkanTexture::CreateImage(VkPhysicalDevice _physicalDevice)
	{
		VkImageCreateInfo imageCreateInfo{};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.extent.width = width_;
		imageCreateInfo.extent.height = height_;
		imageCreateInfo.extent.depth = 1;
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.format = format_;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.flags = 0;
		vkCreateImage(logicalDevice_, &imageCreateInfo, nullptr, &image_) >> VulkanResultChecker::GetInstance();

		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements(logicalDevice_, image_, &memoryRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memoryRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryTypeIndex(_physicalDevice, memoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		vkAllocateMemory(logicalDevice_, &allocInfo, nullptr, &imageMemory_) >> VulkanResultChecker::GetInstance();
		vkBindImageMemory(logicalDevice_, image_, imageMemory_, 0);
	}

	void VulkanTexture::CreateImageView()
	{
		VkImageViewCreateInfo imageViewCreateInfo{};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.image = image_;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = format_;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;
		vkCreateImageView(logicalDevice_, &imageViewCreateInfo, nullptr, &imageView_) >> VulkanResultChecker::GetInstance();
	}

	void VulkanTexture::CreateSampler(VkPhysicalDevice _physicalDevice)
	{
		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(_physicalDevice, &properties);

		VkSamplerCreateInfo samplerCreateInfo{};
		samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.anisotropyEnable = VK_TRUE;
		samplerCreateInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
		samplerCreateInfo.compareEnable = VK_FALSE;
		samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerCreateInfo.mipLodBias = 0.0f;
		samplerCreateInfo.minLod = 0.0f;
		samplerCreateInfo.maxLod = 0.0f;
		vkCreateSampler(logicalDevice_, &samplerCreateInfo, nullptr, &sampler_) >> VulkanResultChecker::GetInstance();
	}

	VkCommandBuffer BeginDisposableCommandBuffer(VkDevice _logicalDevice, VkCommandPool _commandPool)
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = _commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(_logicalDevice, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);
		return commandBuffer;
	}

	void EndDisposableCommandBuffer(VkDevice _logicalDevice, VkCommandBuffer _commandBuffer, VkQueue _graphicsQueue, VkCommandPool _commandPool)
	{
		vkEndCommandBuffer(_commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &_commandBuffer;

		vkQueueSubmit(_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(_graphicsQueue);

		vkFreeCommandBuffers(_logicalDevice, _commandPool, 1, &_commandBuffer);
	}

	void VulkanTexture::TransitImageLayout(VkFormat _format, VkImageLayout _oldLayout, VkImageLayout _newLayout, VkQueue _graphicsQueue, VkCommandPool _commandPool)
	{
		VkCommandBuffer commandBuffer = BeginDisposableCommandBuffer(logicalDevice_, _commandPool);

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = _oldLayout;
		barrier.newLayout = _newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image_;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		
		VkPipelineStageFlags srcStageFlags = 0;
		VkPipelineStageFlags dstStageFlags = 0;

		if (_oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && _newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			srcStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dstStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (_oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && _newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			srcStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
			dstStageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else
		{
			throw std::runtime_error("unsupported transition layouts");
		}

		vkCmdPipelineBarrier(commandBuffer, srcStageFlags, dstStageFlags, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		EndDisposableCommandBuffer(logicalDevice_, commandBuffer, _graphicsQueue, _commandPool);
	}

	void VulkanTexture::CopyBufferToImage(VkBuffer _buffer, VkQueue _graphicsQueue, VkCommandPool _commandPool)
	{
		VkCommandBuffer commandBuffer = BeginDisposableCommandBuffer(logicalDevice_, _commandPool);

		VkBufferImageCopy imageCopy{};
		imageCopy.bufferOffset = 0;
		imageCopy.bufferRowLength = 0;
		imageCopy.bufferImageHeight = 0;
		imageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageCopy.imageSubresource.mipLevel = 0;
		imageCopy.imageSubresource.baseArrayLayer = 0;
		imageCopy.imageSubresource.layerCount = 1;
		imageCopy.imageOffset = { 0, 0, 0 };
		imageCopy.imageExtent = { width_, height_, 1 };
		vkCmdCopyBufferToImage(commandBuffer, _buffer, image_, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);

		EndDisposableCommandBuffer(logicalDevice_, commandBuffer, _graphicsQueue, _commandPool);
	}
}