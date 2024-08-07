#pragma once
#include "vulkan_shader_binding.h"
#include "io/graphics/texture.h"
#include "io/file/image.h"

namespace io::graphics
{
	class VulkanTexture : public Texture, public VulkanShaderBinding
	{
	private:
		VkDevice logicalDevice_ = VK_NULL_HANDLE;
		VkImage image_ = VK_NULL_HANDLE;
		VkImageView imageView_ = VK_NULL_HANDLE;
		VkDeviceMemory imageMemory_ = VK_NULL_HANDLE;
		VkSampler sampler_ = VK_NULL_HANDLE;
		VkFormat format_ = VK_FORMAT_UNDEFINED;
		uint32_t width_ = 0;
		uint32_t height_ = 0;
		VkDescriptorImageInfo imageInfo_;

	public:
		VulkanTexture(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, VkQueue _graphicsQueue, VkCommandPool _commandPool, const Texture::Layout& _textureLayout);
		~VulkanTexture();

	public:
		virtual uint32_t GetWidth() const override;
		virtual uint32_t GetHeight() const override;

		virtual VkDescriptorSetLayoutBinding GetDescriptorLayout() const override;
		virtual VkWriteDescriptorSet GetDescriptorWrite(VkDescriptorSet _descriptorSet) const override;

	private:
		file::Image LoadImage(std::string_view _path);
		void CreateStagingBuffer(VkPhysicalDevice _physicalDevice, const file::Image& _image, VkBuffer& _outStagingBuffer, VkDeviceMemory& _outBufferMemory);
		void CreateImage(VkPhysicalDevice _physicalDevice);
		void CreateImageView();
		void CreateSampler(VkPhysicalDevice _physicalDevice);
		void TransitImageLayout(VkFormat _format, VkImageLayout _oldLayout, VkImageLayout _newLayout, VkQueue _graphicsQueue, VkCommandPool _commandPool);
		void CopyBufferToImage(VkBuffer _buffer, VkQueue _graphicsQueue, VkCommandPool _commandPool);
	};
}