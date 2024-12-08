#pragma once
#include "vulkan/vulkan.h"
#include "graphics/texture.h"
#include "file/image.h"
#include <mutex>

namespace graphics
{
	class VulkanTexture : public Texture
	{
	public:
		struct Initializer
		{
			VkDevice logicalDevice_;
			VkPhysicalDevice physicalDevice_;
			VkQueue graphicsQueue_;
			VkCommandPool commandPool_;
		};

	private:
		inline static std::once_flag placeholderInitialized_;
		inline static file::Image placeholder_;

		VkDevice logicalDevice_;
		VkPhysicalDevice physicalDevice_;
		VkQueue graphicsQueue_;
		VkCommandPool commandPool_;

		VkImage image_ = VK_NULL_HANDLE;
		VkImageView imageView_ = VK_NULL_HANDLE;
		VkDeviceMemory imageMemory_ = VK_NULL_HANDLE;
		VkSampler sampler_ = VK_NULL_HANDLE;
		VkFormat format_;
		uint32_t width_ = 0;
		uint32_t height_ = 0;
		VkDescriptorImageInfo imageInfo_{};
		std::shared_ptr<class VulkanTextureBinding> bindingImpl_;

		std::unique_ptr<file::Image> deferredImage_; // declared as pointer to free unnecessary memory

	public:
		VulkanTexture(Initializer _initializer, const Texture::Layout& _textureLayout);
		~VulkanTexture();

	public:
		virtual std::shared_ptr<BindingImpl> GetBindingImpl() const override;

		virtual uint32_t GetWidth() const override;
		virtual uint32_t GetHeight() const override;

	private:
		void Initialize(VkPhysicalDevice _physicalDevice, VkQueue _graphicsQueue, VkCommandPool _commandPool, file::Image& _image);
		void CreateStagingBuffer(VkPhysicalDevice _physicalDevice, const file::Image& _image, VkBuffer& _outStagingBuffer, VkDeviceMemory& _outBufferMemory);
		void CreateImage(VkPhysicalDevice _physicalDevice);
		void CreateImageView();
		void CreateSampler(VkPhysicalDevice _physicalDevice);
		void TransitImageLayout(VkFormat _format, VkImageLayout _oldLayout, VkImageLayout _newLayout, VkQueue _graphicsQueue, VkCommandPool _commandPool);
		void CopyBufferToImage(VkBuffer _buffer, VkQueue _graphicsQueue, VkCommandPool _commandPool);
	};
}