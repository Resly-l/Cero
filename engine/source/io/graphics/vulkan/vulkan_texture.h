#pragma once
#include "io/graphics/graphics_api.h"
#include "io/graphics/texture.h"
#include <vulkan/vulkan.h>

namespace io::graphics
{
	class VulkanTexture : public Texture
	{
	private:
		VkDevice logicalDevice_ = VK_NULL_HANDLE;
		VkImage image_ = VK_NULL_HANDLE;
		VkDeviceMemory imageMemory_ = VK_NULL_HANDLE;
		uint32_t width_ = 0;
		uint32_t height_ = 0;

	public:
		VulkanTexture(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, VkQueue _graphicsQueue, VkCommandPool _commandPool, const Texture::Layout& _textureLayout);
		~VulkanTexture();

	public:
		virtual uint32_t GetWidth() const override;
		virtual uint32_t GetHeight() const override;

	private:
		void CreateStagingBuffer(VkPhysicalDevice _physicalDevice, const file::Image& _image, VkBuffer& _outStagingBuffer, VkDeviceMemory& _outBufferMemory);
		void CreateImage(VkPhysicalDevice _physicalDevice);
		void TransitImageLayout(VkFormat _format, VkImageLayout _oldLayout, VkImageLayout _newLayout, VkQueue _graphicsQueue, VkCommandPool _commandPool);
		void CopyBufferToImage(VkBuffer _buffer, VkQueue _graphicsQueue, VkCommandPool _commandPool);
	};
}