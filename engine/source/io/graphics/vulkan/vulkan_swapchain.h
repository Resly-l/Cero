#pragma once
#include <vulkan/vulkan.h>
#include "utility/stl.h"

namespace io::graphics
{
	class VulkanDevice;

	class VulkanSwapChain
	{
	private:
		VkSwapchainKHR instance_;
		std::vector<VkImage> images_;
		std::vector<VkImageView> imageViews_;
		std::vector<VkFramebuffer> framebuffers_;

		uint32_t width_;
		uint32_t height_;

	public:
		void Initialize(const VulkanDevice& _device, VkSurfaceKHR _surface, uint32_t _width, uint32_t _height);
		void Release(const VulkanDevice& _device);

		VkSwapchainKHR GetInstance() const;
		const std::vector<VkFramebuffer>& GetFrameBuffers() const;

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;

	private:
		void CreateFrameBuffer(const VulkanDevice& _device);
	};
}