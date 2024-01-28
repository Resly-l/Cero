#include "vulkan_bootstrap.h"
#include "thirdparty/glfw/glfw3.h"

namespace io::graphics
{
	VkInstance VulkanBootstrap::CreateVulkanInstance()
	{
		VkInstance instance{};

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

		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
		{
			throw std::exception("failed to create instance!");
		}

		return instance;
	}

	VkSurfaceKHR VulkanBootstrap::CreateSurface(VkInstance _instnace, GLFWwindow* _window)
	{
		VkSurfaceKHR surface{};

		if (glfwCreateWindowSurface(_instnace, _window, nullptr, &surface) != VK_SUCCESS)
		{
			throw std::exception("failed to create window surface");
		}

		return surface;
	}

	VkPhysicalDevice VulkanBootstrap::SelectPhysicalDevice(VkInstance _instance)
	{
		auto IsDeviceSuitable = [](VkPhysicalDevice _device)
			{
				VkPhysicalDeviceProperties deviceProperties;
				vkGetPhysicalDeviceProperties(_device, &deviceProperties);

				if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				{
					return false;
				}

				uint32_t extensionCount;
				vkEnumerateDeviceExtensionProperties(_device, nullptr, &extensionCount, nullptr);

				std::vector<VkExtensionProperties> availableExtensions(extensionCount);
				vkEnumerateDeviceExtensionProperties(_device, nullptr, &extensionCount, availableExtensions.data());

				std::vector<const char*> requiredExtensionNames = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, };

				for (const std::string extensionName : requiredExtensionNames)
				{
					auto it = std::find_if(availableExtensions.begin(), availableExtensions.end(),
						[extensionName](const VkExtensionProperties& extension)
						{
							return extensionName == extension.extensionName;
						});

					if (it == availableExtensions.end())
					{
						return false;
					};
				}

				return true;
			};

		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

		for (const auto& device : devices)
		{
			if (IsDeviceSuitable(device))
			{
				return device;
			}
		}

		throw std::exception("failed to find GPU with Vulkan support");
		return VkPhysicalDevice{};
	}

	uint32_t VulkanBootstrap::FindGraphicsQueueIndex(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface)
	{
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount, queueFamilies.data());

		for (uint32_t i = 0; i < queueFamilyCount; i++)
		{
			if ((queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
			{
				return i;
			}
		}

		throw std::exception("failed to find graphics queue");
	}

	uint32_t VulkanBootstrap::FindPresentQueueIndex(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface)
	{
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount, queueFamilies.data());

		for (uint32_t i = 0; i < queueFamilyCount; i++)
		{
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice, i, _surface, &presentSupport);

			if (presentSupport)
			{
				return i;
			}
		}

		throw std::exception("failed to find present queue");
	}

	VkDevice VulkanBootstrap::CreateLogicalDevice(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface)
	{
		VkDevice logicalDevice{};

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> queueFamilyIndices = { FindGraphicsQueueIndex(_physicalDevice, _surface), FindPresentQueueIndex(_physicalDevice, _surface) };

		for (uint32_t index : queueFamilyIndices)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = index;
			queueCreateInfo.queueCount = 1;
			float queuePriority = 1.0f;
			queueCreateInfo.pQueuePriorities = &queuePriority;

			queueCreateInfos.push_back(queueCreateInfo);
		}

		const std::vector<const char*> requiredExtensionNames = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		VkPhysicalDeviceFeatures deviceFeatures{};
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = (uint32_t)requiredExtensionNames.size();
		createInfo.ppEnabledExtensionNames = requiredExtensionNames.data();
		createInfo.enabledLayerCount = 0;

		if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS)
		{
			throw std::exception("failed to create logical device");
		}

		return logicalDevice;
	}

	std::vector<VkSurfaceFormatKHR> VulkanBootstrap::GetAvailableSurfaceFormats(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface)
	{
		std::vector<VkSurfaceFormatKHR> availableFormats;
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, _surface, &formatCount, nullptr);

		if (formatCount == 0)
		{
			throw std::exception("failed to select surface format");
		}

		availableFormats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, _surface, &formatCount, availableFormats.data());

		return availableFormats;
	}

	VkSurfaceFormatKHR VulkanBootstrap::FindBestSurfaceFormat(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface)
	{
		for (const auto& format : GetAvailableSurfaceFormats(_physicalDevice, _surface))
		{
			VkImageFormatProperties formatProperties{};
			if (vkGetPhysicalDeviceImageFormatProperties(
				_physicalDevice,
				format.format,
				VK_IMAGE_TYPE_2D,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
				VK_IMAGE_USAGE_STORAGE_BIT |
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				0,
				&formatProperties
			) == VK_SUCCESS)
			{
				return format;
			}
		}

		return VkSurfaceFormatKHR{};
	}

	VkCommandPool VulkanBootstrap::CreateCommandPool(VkDevice _logicalDevice, uint32_t _graphicsQueueIndex)
	{
		VkCommandPool commandPool{};

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = _graphicsQueueIndex;

		if (vkCreateCommandPool(_logicalDevice, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
		{
			throw std::exception("failed to create command pool!");
		}

		return commandPool;
	}

	std::vector<VkCommandBuffer> VulkanBootstrap::CreateCommandBuffers(VkDevice _logicalDevice, VkCommandPool _commandPool, size_t _numBuffers)
	{
		std::vector<VkCommandBuffer> commandBuffers(_numBuffers);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = _commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)_numBuffers;

		if (vkAllocateCommandBuffers(_logicalDevice, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::exception("failed to allocate command buffers");
		}

		return commandBuffers;
	}

	VkRenderPass VulkanBootstrap::CreateRenderPass(VkDevice _logicalDevice, VkSurfaceFormatKHR _format)
	{
		VkRenderPass renderpass{};
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = _format.format;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(_logicalDevice, &renderPassInfo, nullptr, &renderpass) != VK_SUCCESS)
		{
			throw std::exception("failed to create render pass");
		}

		return renderpass;
	}

	std::vector<VkSemaphore> VulkanBootstrap::CreateSemaphores(VkDevice _logicalDevice, size_t _numSemaphores)
	{
		std::vector<VkSemaphore> semaphores(_numSemaphores);
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		for (auto& semaphore : semaphores)
		{
			if (vkCreateSemaphore(_logicalDevice, &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS)
			{
				throw std::exception("failed to allocate semaphores");
			}
		}

		return semaphores;
	}

	std::vector<VkFence> VulkanBootstrap::CreateFences(VkDevice _logicalDevice, size_t _numFences)
	{
		std::vector<VkFence> fences(_numFences);
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (auto& fence : fences)
		{
			if (vkCreateFence(_logicalDevice, &fenceInfo, nullptr, &fence) != VK_SUCCESS)
			{
				throw std::exception("failed to allocate fences");
			}
		}

		return fences;
	}

	VkSwapchainKHR VulkanBootstrap::CreateSwapchain(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface)
	{
		VkSwapchainKHR swapchain;
		VkSurfaceCapabilitiesKHR capabilities{};
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevice, _surface, &capabilities);

		const auto& format = FindBestSurfaceFormat(_physicalDevice, _surface);

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = _surface;
		createInfo.minImageCount = capabilities.minImageCount + 1;
		createInfo.imageFormat = format.format;
		createInfo.imageColorSpace = format.colorSpace;
		createInfo.imageExtent = capabilities.currentExtent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		std::set<uint32_t> uniqueQueueFamilyIndices({ FindGraphicsQueueIndex(_physicalDevice, _surface), FindPresentQueueIndex(_physicalDevice, _surface) });
		std::vector<uint32_t> queueFamilyIndices(uniqueQueueFamilyIndices.begin() ,uniqueQueueFamilyIndices.end());
		createInfo.queueFamilyIndexCount = queueFamilyIndices.size();
		createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
		createInfo.imageSharingMode = (createInfo.queueFamilyIndexCount > 1) ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
		createInfo.preTransform = capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
		createInfo.clipped = VK_TRUE;

		if (vkCreateSwapchainKHR(_logicalDevice, &createInfo, nullptr, &swapchain) != VK_SUCCESS)
		{
			throw std::exception("failed to create swap chain");
		}

		return swapchain;
	}

	std::vector<VkImageView> VulkanBootstrap::CreateImageViews(VkDevice _logicalDevice, const std::vector<VkImage>& _images, VkSurfaceFormatKHR _format)
	{
		std::vector<VkImageView> imagesViews(_images.size());

		for (size_t i = 0; i < _images.size(); i++)
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = _images[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = _format.format;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(_logicalDevice, &createInfo, nullptr, &imagesViews[i]) != VK_SUCCESS)
			{
				throw std::exception("failed to create image views");
			}
		}

		return imagesViews;
	}

	std::vector<VkImageView> VulkanBootstrap::CreateSwapchainImageViews(VkDevice _logicalDevice, VkSwapchainKHR _swapchain, VkSurfaceFormatKHR _format)
	{
		std::vector<VkImage> images;
		uint32_t imageCount = 0;
		vkGetSwapchainImagesKHR(_logicalDevice, _swapchain, &imageCount, nullptr);
		images.resize(imageCount);
		if (vkGetSwapchainImagesKHR(_logicalDevice, _swapchain, &imageCount, images.data()) != VK_SUCCESS)
		{
			throw std::exception("failed to get swap chain images");
		}

		return CreateImageViews(_logicalDevice, images, _format);
	}

	std::vector<VkFramebuffer> VulkanBootstrap::CreateFramebuffers(VkDevice _logicalDevice, const std::vector<VkImageView>& _imageViews, VkRenderPass _renderPass, uint32_t _width, uint32_t _height)
	{
		std::vector<VkFramebuffer> framebuffers(_imageViews.size());

		for (size_t i = 0; i < _imageViews.size(); i++)
		{
			VkImageView attachments[] =
			{
				_imageViews[i]
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = _renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.flags;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = _width;
			framebufferInfo.height = _height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(_logicalDevice, &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS)
			{
				throw std::exception("failed to create framebuffer");
			}
		}

		return framebuffers;
	}
}