#pragma once
#include "vulkan_shader_binding.h"
#include "graphics/texture.h"
#include "file/image.h"
#include <atomic>

namespace graphics
{
	class VulkanTexture : public Texture
	{
	private:
		inline static std::unique_ptr<file::Image> placeholder_;

		VkDevice logicalDevice_;
		VkPhysicalDevice physicalDevice_;
		VkQueue graphicsQueue_;
		VkCommandPool commandPool_;

		VkImage image_;
		VkImageView imageView_;
		VkDeviceMemory imageMemory_;
		VkSampler sampler_;
		VkFormat format_;
		uint32_t width_ = 0;
		uint32_t height_ = 0;
		VkDescriptorImageInfo imageInfo_{};
		VkShaderStageFlags stage_{};

		std::unique_ptr<file::Image> imageFile_;
		std::atomic_bool isLoaded_ = false;

	public:
		VulkanTexture(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, VkQueue _graphicsQueue, VkCommandPool _commandPool, const Texture::Layout& _textureLayout);
		~VulkanTexture();

	public:
		virtual bool IsPendingUpdate() const override;
		virtual void Update() override;
		virtual std::shared_ptr<ShaderBinding> GetShaderBinding() const override;
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