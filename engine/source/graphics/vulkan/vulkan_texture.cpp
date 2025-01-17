#include "vulkan_texture.h"
#include "vulkan_result.hpp"
#include "vulkan_utility.h"
#include "vulkan_shader_binding.h"
#include "thread/thread_pool.h"
#include "file/path.generated.h"
#include "utility/log.h"

using utility::Log;

namespace graphics
{
	class VulkanTextureBinding : public VulkanShaderBinding
	{
	public:
		VkDescriptorImageInfo imageInfo_{};

	public:
		virtual void FillBindingInfo(VkWriteDescriptorSet& _WriteDescriptorSet) const override
		{
			_WriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			_WriteDescriptorSet.pImageInfo = &imageInfo_;
		}
	};

	VulkanTexture::VulkanTexture(Initializer _initializer, const Texture::Layout& _layout)
		: logicalDevice_(_initializer.logicalDevice_)
		, physicalDevice_(_initializer.physicalDevice_)
		, graphicsQueue_(_initializer.graphicsQueue_)
		, commandPool_(_initializer.commandPool_)
	{
		std::call_once(placeholderInitialized_, []()
			{
				if (!placeholder_.Load(std::string(PATH_ENGINE_ASSET) + std::string("image/placeholder.png")))
				{
					throw std::runtime_error(std::string("failed to load placeholder image"));
				}
			});

		format_ = VulkanTypeConverter::Convert(_layout.format_);

		if (_layout.initializationType_ == Texture::InitializationType::FILE)
		{
			Initialize(physicalDevice_, graphicsQueue_, commandPool_, placeholder_);

			thread::ThreadPool::EnqueueTask([&]()
				{
					deferredImage_ = std::make_unique<file::Image>();
					if (!deferredImage_->Load(_layout.imagePath_))
					{
						std::cout << Log::Format(Log::Category::file, Log::Level::error, "failed to load image, path : " + std::string(_layout.imagePath_)) << std::endl;
						return;
					}

					vkQueueWaitIdle(graphicsQueue_);

					vkDestroySampler(logicalDevice_, sampler_, nullptr);
					vkFreeMemory(logicalDevice_, imageMemory_, nullptr);
					vkDestroyImageView(logicalDevice_, imageView_, nullptr);
					vkDestroyImage(logicalDevice_, image_, nullptr);

					Initialize(physicalDevice_, graphicsQueue_, commandPool_, *deferredImage_);
					deferredImage_ = nullptr;
				});
		}
		else if (_layout.initializationType_ == Texture::InitializationType::BUFFER)
		{
			if (!_layout.buffer_.has_value())
			{
				std::cout << Log::Format(Log::Category::file, Log::Level::error, "tried to load image with buffer but has no buffer" + std::string(_layout.imagePath_)) << std::endl;
				Initialize(physicalDevice_, graphicsQueue_, commandPool_, placeholder_);
			}
			else
			{
				auto image = _layout.buffer_.value();
				Initialize(physicalDevice_, graphicsQueue_, commandPool_, image);
			}
		}
	}

	VulkanTexture::~VulkanTexture()
	{
		vkDestroySampler(logicalDevice_, sampler_, nullptr);
		vkFreeMemory(logicalDevice_, imageMemory_, nullptr);
		vkDestroyImageView(logicalDevice_, imageView_, nullptr);
		vkDestroyImage(logicalDevice_, image_, nullptr);
	}

	std::shared_ptr<ShaderBinding::BindingImpl> VulkanTexture::GetBindingImpl() const
	{
		return bindingImpl_;
	}

	uint32_t VulkanTexture::GetWidth() const
	{
		return width_;
	}

	uint32_t VulkanTexture::GetHeight() const
	{
		return height_;
	}

	void VulkanTexture::Initialize(VkPhysicalDevice _physicalDevice, VkQueue _graphicsQueue, VkCommandPool _commandPool, file::Image& _image)
	{
		width_ = _image.width_;
		height_ = _image.height_;

		VkBuffer stagingBuffer = VK_NULL_HANDLE;
		VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

		CreateStagingBuffer(_physicalDevice, _image, stagingBuffer, stagingBufferMemory);
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

		if (!bindingImpl_)
		{
			bindingImpl_ = std::make_shared<VulkanTextureBinding>();
			bindingImpl_->imageInfo_ = imageInfo_;
		}
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
		vkCreateImage(logicalDevice_, &imageCreateInfo, nullptr, &image_) >> VulkanResultChecker::Get();

		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements(logicalDevice_, image_, &memoryRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memoryRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryTypeIndex(_physicalDevice, memoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		vkAllocateMemory(logicalDevice_, &allocInfo, nullptr, &imageMemory_) >> VulkanResultChecker::Get();
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
		vkCreateImageView(logicalDevice_, &imageViewCreateInfo, nullptr, &imageView_) >> VulkanResultChecker::Get();
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
		vkCreateSampler(logicalDevice_, &samplerCreateInfo, nullptr, &sampler_) >> VulkanResultChecker::Get();
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