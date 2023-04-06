#pragma once
#include <vulkan/vulkan.h>
#include "ThirdParty/glfw/glfw3.h"

#include "Renderer/IGraphicsAPI.h"
#include "Renderer/PipelineState.h"


class VulkanAPI final : public IGraphicsAPI
{
private:
	struct QueueFamilyIndex
	{
		std::optional<uint32_t> graphicsFamilyIndex;
		std::optional<uint32_t> presentFamilyIndex;
	};

private:
	inline static std::vector<const char*> requiredExtensionNames;

private:
	GLFWwindow* window;

	VkInstance instance;
	VkSurfaceKHR surface;

	VkPhysicalDevice physicalDevice;
	VkDevice logicalDevice;

	VkQueue graphicsQueue;
	VkQueue presentationQueue;

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;

	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	VkFence inFlightFence;

	int width;
	int height;

public:
	VulkanAPI(GLFWwindow* window_in, int width_in, int height_in);
	~VulkanAPI();

public:
	void Execute();

private:
	void CreateVulkanInstance();
	void CreateSurface();
	void SelectPhysicalDevice();
	void CreateLogicalDevice(const QueueFamilyIndex& queueFamilyIndex);
	void CreateSwapChain(const QueueFamilyIndex& queueFamilyIndex);
	void InitializeSwapChainImages();

	void CreatePipeline();
	void CreateFrameBuffer();
	void CreateCommandPool(const QueueFamilyIndex& queueFamilyIndex);
	void CreateCommandBuffer();
	void CreateSyncObjects();

	static bool IsDeviceSuitable(VkPhysicalDevice device);
	QueueFamilyIndex GetQueueFamilyIndex();

	void RecordCommandBuffer(const size_t bufferIndex);
};