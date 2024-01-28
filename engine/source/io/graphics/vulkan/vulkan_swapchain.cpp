#include "vulkan_swapchain.h"
#include "vulkan_device.h"
#include "vulkan_bootstrap.h"

namespace io::graphics
{
	void VulkanSwapChain::Initialize(const VulkanDevice& _device, VkSurfaceKHR _surface, uint32_t _width, uint32_t _height)
	{
		width_ = _width;
		height_ = _height;

		auto surfaceFormat = VulkanBootstrap::FindBestSurfaceFormat(_device.GetPhysicalDevice(), _surface);

		instance_ = VulkanBootstrap::CreateSwapchain(_device.GetLogicalDevice(), _device.GetPhysicalDevice(), _surface);
		imageViews_ = VulkanBootstrap::CreateSwapchainImageViews(_device.GetLogicalDevice(), instance_, surfaceFormat);
		framebuffers_ = VulkanBootstrap::CreateFramebuffers(_device.GetLogicalDevice(), imageViews_, _device.GetRenderPass(), width_, height_);
	}

	void VulkanSwapChain::Release(const VulkanDevice& _device)
	{
		for (auto framebuffer : framebuffers_)
		{
			vkDestroyFramebuffer(_device.GetLogicalDevice(), framebuffer, nullptr);
		}

		for (auto imageView : imageViews_)
		{
			vkDestroyImageView(_device.GetLogicalDevice(), imageView, nullptr);
		}

		vkDestroySwapchainKHR(_device.GetLogicalDevice(), instance_, nullptr);
	}

    VkSwapchainKHR VulkanSwapChain::GetInstance() const
    {
		return instance_;
    }

	const std::vector<VkFramebuffer>& VulkanSwapChain::GetFrameBuffers() const
	{
		return framebuffers_;
	}

	uint32_t VulkanSwapChain::GetWidth() const
	{
		return width_;
	}

	uint32_t VulkanSwapChain::GetHeight() const
	{
		return height_;
	}
}