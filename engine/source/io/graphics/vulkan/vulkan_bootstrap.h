#pragma once
#include <vulkan/vulkan.h>
#include "utility/stl.h"

struct GLFWwindow;

namespace io::graphics
{
	// wrapper functions of vkCreate functions
	// so that Vulkan~ classes doesn't have to care much about creation
	// VulkanBootstrap should not know user types
	class VulkanBootstrap
	{
	public:
		static VkInstance CreateVulkanInstance();
		static VkSurfaceKHR CreateSurface(VkInstance _instnace, GLFWwindow* _window);
		static VkPhysicalDevice SelectPhysicalDevice(VkInstance _instance);

		static uint32_t FindGraphicsQueueIndex(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface);
		static uint32_t FindPresentQueueIndex(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface);

		static VkDevice CreateLogicalDevice(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface);

		static std::vector<VkSurfaceFormatKHR> GetAvailableSurfaceFormats(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface);
		static VkSurfaceFormatKHR FindBestSurfaceFormat(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface);

		static VkCommandPool CreateCommandPool(VkDevice _logicalDevice, uint32_t _graphicsQueueIndex);
		static std::vector<VkCommandBuffer> CreateCommandBuffers(VkDevice _logicalDevice, VkCommandPool _commandPool, size_t _numBuffers);
		static VkRenderPass CreateRenderPass(VkDevice _logicalDevice, VkSurfaceFormatKHR _format);

		static std::vector<VkSemaphore> CreateSemaphores(VkDevice _logicalDevice, size_t _numSemaphores);
		static std::vector<VkFence> CreateFences(VkDevice _logicalDevice, size_t _numFences);

		static VkSwapchainKHR CreateSwapchain(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface);
		static std::vector<VkImageView> CreateImageViews(VkDevice _logicalDevice, const std::vector<VkImage>& _images, VkSurfaceFormatKHR _format);
		static std::vector<VkImageView> CreateSwapchainImageViews(VkDevice _logicalDevice, VkSwapchainKHR _swapchain, VkSurfaceFormatKHR _format);
		static std::vector<VkFramebuffer> CreateFramebuffers(VkDevice _logicalDevice, const std::vector<VkImageView>& imageViews, VkRenderPass _renderPass, uint32_t _width, uint32_t _height);
	};

	static_assert(sizeof(VulkanBootstrap) == 1, "VulkanBootstrap must be empty");
}