#include "vulkan_api.h"

namespace io::graphics
{
	VulkanAPI::VulkanAPI(GLFWwindow& _window)
		: window_(_window)
	{
		CreateVulkanInstance();
		CreateSurface();

		device_.Initialize(instance_, surface_);

		int width = 0;
		int height = 0;
		glfwGetFramebufferSize(&window_, &width, &height);
		swapChain_.Initialize(device_, surface_, (uint32_t)width, (uint32_t)height);

		commander_.Intialize(device_);
	}

	VulkanAPI::~VulkanAPI()
	{
		pipeline_.Release(device_);
		commander_.Release(device_);
		swapChain_.Release(device_);
		device_.Release();

		vkDestroySurfaceKHR(instance_, surface_, nullptr);
		vkDestroyInstance(instance_, nullptr);
	}

	void VulkanAPI::BindPipeline(const PipelineState& _pipelineState)
	{
		pipeline_.Release(device_);
		pipeline_.Initialize(device_, _pipelineState, commander_, swapChain_);
	}

	void VulkanAPI::Draw()
	{
		commander_.Execute(device_, swapChain_, pipeline_);
	}

	void VulkanAPI::Present()
	{
		commander_.Present(device_, swapChain_);
	}

	void VulkanAPI::CreateVulkanInstance()
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;
#if _DEBUG
		std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

		createInfo.enabledLayerCount = (uint32_t)validationLayers.size();
		createInfo.ppEnabledLayerNames = validationLayers.data();
#endif

		if (vkCreateInstance(&createInfo, nullptr, &instance_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create instance!");
		}
	}

	void VulkanAPI::CreateSurface()
	{
		if (glfwCreateWindowSurface(instance_, &window_, nullptr, &surface_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface!");
		}
	}
}