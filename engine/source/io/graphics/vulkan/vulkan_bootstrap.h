#pragma once
#include <vulkan/vulkan.h>
#include "utility/stl.h"

struct GLFWwindow;

namespace io::graphics
{
	struct PipelineState;
	enum class ImageFormat;
	enum class ImageOperation;
	enum class ImageLayout;
	enum class ImageUsage;

	// wrapper functions of vkCreate functions
	// so that Vulkan~ classes doesn't have to care much about creation
	class VulkanBootstrap
	{
	public:
		static VkInstance CreateVulkanInstance();
		static VkSurfaceKHR CreateSurface(VkInstance _instnace, GLFWwindow* _window);
		static VkPhysicalDevice SelectPhysicalDevice(VkInstance _instance);

		static uint32_t FindGraphicsQueueFamilyIndex(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface);
		static uint32_t FindPresentQueueFamilyIndex(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface);

		static VkDevice CreateLogicalDevice(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface);

		static std::vector<VkSurfaceFormatKHR> GetAvailableSurfaceFormats(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface);
		static VkSurfaceFormatKHR FindBestSurfaceFormat(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface);
		static VkFormat FindBestDepthFormat(VkPhysicalDevice _physicalDevice);

		static VkCommandPool CreateCommandPool(VkDevice _logicalDevice, uint32_t _graphicsQueueIndex);
		static std::vector<VkCommandBuffer> CreateCommandBuffers(VkDevice _logicalDevice, VkCommandPool _commandPool, size_t _numBuffers);
		static VkRenderPass CreateRenderPass(VkDevice _logicalDevice, VkSurfaceFormatKHR _format);

		static std::vector<VkSemaphore> CreateSemaphores(VkDevice _logicalDevice, size_t _numSemaphores);
		static std::vector<VkFence> CreateFences(VkDevice _logicalDevice, size_t _numFences);

		static VkSurfaceCapabilitiesKHR GetSurfaceCapabilities(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface);
		static VkSwapchainKHR CreateSwapchain(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface);
		static VkImage CreateImage(VkDevice _logicalDevice, VkSurfaceFormatKHR _format, VkImageUsageFlagBits _usage, uint32_t _width, uint32_t _height, uint32_t _depth);
		static VkImageView CreateImageView(VkDevice _logicalDevice, VkImage _image, VkSurfaceFormatKHR _format, VkImageUsageFlagBits _usage);
		static VkFramebuffer CreateFramebuffer(VkDevice _logicalDevice, VkImageView _imageView, VkRenderPass _renderPass, uint32_t _width, uint32_t _height);
		static VkDeviceMemory AllocateImageMemory(VkDevice _logicalDevice, VkImage _image);
		static std::vector<VkImage> CreateImages(VkDevice _logicalDevice, VkSurfaceFormatKHR _format, VkImageUsageFlagBits _usage, uint32_t _width, uint32_t _height, uint32_t _depth, uint32_t _numImages);
		static std::vector<VkImageView> CreateImageViews(VkDevice _logicalDevice, const std::vector<VkImage>& _images, VkSurfaceFormatKHR _format, VkImageUsageFlagBits _usage);
		static std::vector<VkImageView> CreateSwapchainImageViews(VkDevice _logicalDevice, VkSwapchainKHR _swapchain, VkSurfaceFormatKHR _format);
		static std::vector<VkFramebuffer> CreateFramebuffers(VkDevice _logicalDevice, const std::vector<VkImageView>& _imageViews, VkRenderPass _renderPass, uint32_t _width, uint32_t _height);

		static VkBuffer CreateBuffer(VkDevice _logicalDevice, size_t _size, VkBufferUsageFlags _usage);
		static VkDeviceMemory AllocateMemory(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, VkBuffer _buffer, VkMemoryPropertyFlags _properties);
		static void CopyBuffer(VkDevice _logicalDevice, VkCommandPool _commandPool, VkQueue _queue,  VkBuffer _source, VkBuffer _destination, size_t _size);

		static std::pair<VkBuffer, VkDeviceMemory> CreateVertexBuffer(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, VkCommandPool _commandPool, VkQueue _queue, const void* _vertexData, uint32_t _bufferSize);
		static PipelineState CreatePresentationPipelineState();
		static VkPipeline CreatePipeline(VkDevice _logicalDevice, const PipelineState& _pipelineState, VkRenderPass _renderPass, uint32_t _numColorAttachments);

		static VkSurfaceFormatKHR ConvertFormat(const ImageFormat& _format);
		static VkImageUsageFlagBits ConvertImageUsage(const ImageUsage& _usage);
		static VkAttachmentLoadOp ConvertAttachmentLoadOperation(const ImageOperation& _opeartion);
		static VkAttachmentStoreOp ConvertAttachmentStoreOperation(const ImageOperation& _opeartion);
		static VkImageLayout ConvertImageLayout(const ImageLayout& _imageLayout);
	};

	static_assert(sizeof(VulkanBootstrap) == 1, "VulkanBootstrap must be empty");
}