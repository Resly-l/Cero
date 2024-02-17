#include "vulkan_bootstrap.h"
#include "thirdparty/glfw/glfw3.h"
#include "io/graphics/graphics_api.h"
#include "io/file/file_interface.h"
#include "core/math/vector.h"

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

	uint32_t VulkanBootstrap::FindGraphicsQueueFamilyIndex(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface)
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

	uint32_t VulkanBootstrap::FindPresentQueueFamilyIndex(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface)
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
		std::set<uint32_t> queueFamilyIndices = { FindGraphicsQueueFamilyIndex(_physicalDevice, _surface), FindPresentQueueFamilyIndex(_physicalDevice, _surface) };

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

	VkFormat VulkanBootstrap::FindBestDepthFormat(VkPhysicalDevice _physicalDevice)
	{
		std::vector<VkFormat> formats = {
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM
		};

		for (auto& format : formats)
		{
			VkFormatProperties formatProperties{};
			vkGetPhysicalDeviceFormatProperties(_physicalDevice, format, &formatProperties);
			if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
			{
				return format;
			}
		}

		return VK_FORMAT_UNDEFINED;
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

	VkSurfaceCapabilitiesKHR VulkanBootstrap::GetSurfaceCapabilities(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface)
	{
		VkSurfaceCapabilitiesKHR capabilities{};
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevice, _surface, &capabilities);

		return capabilities;
	}

	VkSwapchainKHR VulkanBootstrap::CreateSwapchain(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface)
	{
		VkSwapchainKHR swapchain;
		VkSurfaceCapabilitiesKHR capabilities = GetSurfaceCapabilities(_physicalDevice, _surface);;

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
		std::set<uint32_t> uniqueQueueFamilyIndices({ FindGraphicsQueueFamilyIndex(_physicalDevice, _surface), FindGraphicsQueueFamilyIndex(_physicalDevice, _surface) });
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

	VkImage VulkanBootstrap::CreateImage(VkDevice _logicalDevice, VkSurfaceFormatKHR _format, VkImageUsageFlagBits _usage, uint32_t _width, uint32_t _height, uint32_t _depth)
	{
		VkImage image{};
		VkImageCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		//createInfo.flags = VK_IMAGE_CREATE_SPARSE_BINDING_BIT;
		createInfo.imageType = VK_IMAGE_TYPE_2D;
		createInfo.format = _format.format;
		createInfo.extent.width = _width;
		createInfo.extent.height = _height;
		createInfo.extent.depth = _depth;
		createInfo.mipLevels = 1;
		createInfo.arrayLayers = 1;
		createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		createInfo.usage = _usage | VK_IMAGE_USAGE_SAMPLED_BIT;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		/*createInfo.queueFamilyIndexCount;
		createInfo.pQueueFamilyIndices;*/
		createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		if (vkCreateImage(_logicalDevice, &createInfo, nullptr, &image) != VK_SUCCESS)
		{
			throw std::exception("failed to creaet image view");
		}

		return image;
	}

	std::vector<VkImage> VulkanBootstrap::CreateImages(VkDevice _logicalDevice, VkSurfaceFormatKHR _format, VkImageUsageFlagBits _usage, uint32_t _width, uint32_t _height, uint32_t _depth, uint32_t _numImages)
	{
		std::vector<VkImage> images(_numImages);

		for (auto& image : images)
		{
			image = CreateImage(_logicalDevice, _format, _usage, _width, _height, _depth);
		}

		return images;
	}

	VkDeviceMemory VulkanBootstrap::AllocateImageMemory(VkDevice _logicalDevice, VkImage _image)
	{
		VkMemoryRequirements memoryRequirements{};
		vkGetImageMemoryRequirements(_logicalDevice, _image, &memoryRequirements);

		VkMemoryAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocateInfo.allocationSize = memoryRequirements.size;
		//allocateInfo.memoryTypeIndex = memoryRequirements.memoryTypeBits;

		VkDeviceMemory imageMemory{};
		vkAllocateMemory(_logicalDevice, &allocateInfo, nullptr, &imageMemory);
		vkBindImageMemory(_logicalDevice, _image, imageMemory, 0);

		return imageMemory;
	}

	VkImageView VulkanBootstrap::CreateImageView(VkDevice _logicalDevice, VkImage _image, VkSurfaceFormatKHR _format, VkImageUsageFlagBits _usage)
	{
		VkImageAspectFlags aspectMask = 0;
		if (_usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
		{
			aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}
		if (_usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			if (_format.format >= VK_FORMAT_D16_UNORM_S8_UINT)
			{
				aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}

		VkImageView imageView{};
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = _image;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = _format.format;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = aspectMask;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(_logicalDevice, &createInfo, nullptr, &imageView) != VK_SUCCESS)
		{
			throw std::exception("failed to create image views");
		}

		return imageView;
	}

	std::vector<VkImageView> VulkanBootstrap::CreateImageViews(VkDevice _logicalDevice, const std::vector<VkImage>& _images, VkSurfaceFormatKHR _format, VkImageUsageFlagBits _usage)
	{
		std::vector<VkImageView> imagesViews(_images.size());

		for (size_t i = 0; i < _images.size(); i++)
		{
			imagesViews[i] = CreateImageView(_logicalDevice, _images[i], _format, _usage);
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

		return CreateImageViews(_logicalDevice, images, _format, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
	}

	VkFramebuffer VulkanBootstrap::CreateFramebuffer(VkDevice _logicalDevice, VkImageView _imageView, VkRenderPass _renderPass, uint32_t _width, uint32_t _height)
	{
		VkFramebuffer framebuffer{};
		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = _renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.flags;
		framebufferInfo.pAttachments = &_imageView;
		framebufferInfo.width = _width;
		framebufferInfo.height = _height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(_logicalDevice, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS)
		{
			throw std::exception("failed to create framebuffer");
		}

		return framebuffer;
	}

	std::vector<VkFramebuffer> VulkanBootstrap::CreateFramebuffers(VkDevice _logicalDevice, const std::vector<VkImageView>& _imageViews, VkRenderPass _renderPass, uint32_t _width, uint32_t _height)
	{
		std::vector<VkFramebuffer> framebuffers(_imageViews.size());

		for (size_t i = 0; i < _imageViews.size(); i++)
		{
			framebuffers[i] = CreateFramebuffer(_logicalDevice, _imageViews[i], _renderPass, _width, _height);
		}

		return framebuffers;
	}

	VkBuffer VulkanBootstrap::CreateBuffer(VkDevice _logicalDevice, size_t _size, VkBufferUsageFlags _usage)
	{
		VkBuffer buffer;
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = _size;
        bufferInfo.usage = _usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(_logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
		{
            throw std::runtime_error("failed to create buffer");
        }

		return buffer;
	}

	VkDeviceMemory VulkanBootstrap::AllocateMemory(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, VkBuffer _buffer, VkMemoryPropertyFlags _properties)
	{
		auto FindMemoryType = [&](uint32_t _typeFilter, VkMemoryPropertyFlags _flags)
		{
			VkPhysicalDeviceMemoryProperties memProperties;
			vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &memProperties);

			for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
			{
				if ((_typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & _flags) == _flags)
				{
					return i;
				}
			}

			throw std::runtime_error("failed to find suitable memory type!");
		};

		VkDeviceMemory bufferMemory;
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(_logicalDevice, _buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, _properties);

        if (vkAllocateMemory(_logicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
		{
            throw std::runtime_error("failed to allocate buffer memory");
        }

        vkBindBufferMemory(_logicalDevice, _buffer, bufferMemory, 0);
		return bufferMemory;
	}

	void VulkanBootstrap::CopyBuffer(VkDevice _logicalDevice, VkCommandPool _commandPool, VkQueue _queue, VkBuffer _source, VkBuffer _destination, size_t _size)
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = _commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(_logicalDevice, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);
		{
			VkBufferCopy copyRegion{};
			copyRegion.size = _size;
			vkCmdCopyBuffer(commandBuffer, _source, _destination, 1, &copyRegion);
		}
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(_queue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(_queue);

		vkFreeCommandBuffers(_logicalDevice, _commandPool, 1, &commandBuffer);
	}

	std::pair<VkBuffer, VkDeviceMemory> VulkanBootstrap::CreateVertexBuffer(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, VkCommandPool _commandPool, VkQueue _queue, const void* _vertexData, uint32_t _bufferSize)
	{
		VkBuffer buffer{};
		VkDeviceMemory memory{};

		const size_t bufferSize = _bufferSize;
		auto stagingBuffer = VulkanBootstrap::CreateBuffer(_logicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
		auto stagingBufferMemory = VulkanBootstrap::AllocateMemory(_logicalDevice, _physicalDevice, stagingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		void* data;
		vkMapMemory(_logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, _vertexData, (size_t)bufferSize);
		vkUnmapMemory(_logicalDevice, stagingBufferMemory);

		buffer = VulkanBootstrap::CreateBuffer(_logicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
		memory = VulkanBootstrap::AllocateMemory(_logicalDevice, _physicalDevice, buffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        VulkanBootstrap::CopyBuffer(_logicalDevice, _commandPool, _queue, stagingBuffer, buffer, bufferSize);

        vkDestroyBuffer(_logicalDevice, stagingBuffer, nullptr);
        vkFreeMemory(_logicalDevice, stagingBufferMemory, nullptr);

		return std::make_pair(buffer, memory);
	}

	PipelineState VulkanBootstrap::CreatePresentationPipelineState()
	{
		PipelineState pipelineState;

		pipelineState.vertexShaderPath = L"../../../engine/asset/shader/bin/hello_triangle.vert.spv";
		pipelineState.pixelShaderPath = L"../../../engine/asset/shader/bin/hello_triangle.frag.spv";

		pipelineState.viewport_.width_ = 1600;
		pipelineState.viewport_.height_ = 900;

		utility::ByteBuffer::Layout vertexLayout;
		vertexLayout.AddAttribute<math::Vec2<float>>("position");
		vertexLayout.AddAttribute<math::Vec3<float>>("color");
		pipelineState.vertexBuffer.SetLayout(vertexLayout);

		if (auto vertex = pipelineState.vertexBuffer.Add())
		{
			vertex->Get<math::Vec2<float>>("position") = math::Vec2(0.0f, -0.5f);
			vertex->Get<math::Vec3<float>>("color") = math::Vec3(0.98f, 0.75f, 0.85f);
		}

		if (auto vertex = pipelineState.vertexBuffer.Add())
		{
			vertex->Get<math::Vec2<float>>("position") = math::Vec2(0.35f, 0.5f);
			vertex->Get<math::Vec3<float>>("color") = math::Vec3(0.15f, 0.15f, 0.15f);
		}

		if (auto vertex = pipelineState.vertexBuffer.Add())
		{
			vertex->Get<math::Vec2<float>>("position") = math::Vec2(-0.35f, 0.5f);
			vertex->Get<math::Vec3<float>>("color") = math::Vec3(0.69f, 0.22f, 0.25f);
		}

		return pipelineState;
	}

	VkPipeline VulkanBootstrap::CreatePipeline(VkDevice _logicalDevice, const PipelineState& _pipelineState, VkRenderPass _renderPass, uint32_t _numColorAttachments)
	{
		VkPipelineLayout pipelineLayout;
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0; // Optional
		pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

		if (vkCreatePipelineLayout(_logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout");
		}

		std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
		auto vertShaderCode = io::file::FileInterface::LoadFile(_pipelineState.vertexShaderPath, true, true);
		auto fragShaderCode = io::file::FileInterface::LoadFile(_pipelineState.pixelShaderPath, true, true);

		VkShaderModule vertexShaderModule, fragShaderModule;
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = vertShaderCode.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(vertShaderCode.data());

		if (vkCreateShaderModule(_logicalDevice, &createInfo, nullptr, &vertexShaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create vertex shader");
		}

		createInfo.codeSize = fragShaderCode.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(fragShaderCode.data());

		if (vkCreateShaderModule(_logicalDevice, &createInfo, nullptr, &fragShaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create fragment shader");
		}

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertexShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		shaderStageCreateInfos.push_back(vertShaderStageInfo);
		shaderStageCreateInfos.push_back(fragShaderStageInfo);

		VkVertexInputBindingDescription vertexInputBindingDescription{};
		vertexInputBindingDescription.binding = 0;
		vertexInputBindingDescription.stride = (uint32_t)_pipelineState.vertexBuffer.GetElementSize();
		vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;
		for (size_t i = 0; i < _pipelineState.vertexBuffer.GetLayout()->GetNumAttibutes(); i++)
		{
			VkVertexInputAttributeDescription vertexAttributeDescription{};
			vertexAttributeDescription.binding = 0;
			vertexAttributeDescription.location = (uint32_t)i;

			switch (_pipelineState.vertexBuffer.GetLayout()->GetAttributeSize(i))
			{
			case 8: vertexAttributeDescription.format = VK_FORMAT_R32G32_SFLOAT; break;
			case 12: vertexAttributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT; break;
			}

			vertexAttributeDescription.offset = (uint32_t)_pipelineState.vertexBuffer.GetLayout()->GetAttributeOffset(i);

			vertexAttributeDescriptions.push_back(vertexAttributeDescription);
		}

		VkPipelineVertexInputStateCreateInfo vertexInputStateInfo{};
		vertexInputStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputStateInfo.vertexBindingDescriptionCount = 1;
		vertexInputStateInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;
		vertexInputStateInfo.vertexAttributeDescriptionCount = (uint32_t)vertexAttributeDescriptions.size();
		vertexInputStateInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
		inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)_pipelineState.viewport_.width_;
		viewport.height = (float)_pipelineState.viewport_.height_;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor;
		scissor.offset = { 0, 0 };
		scissor.extent = { _pipelineState.viewport_.width_, _pipelineState.viewport_.height_ };

		VkPipelineViewportStateCreateInfo viewportInfo{};
		viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportInfo.viewportCount = 1;
		viewportInfo.pViewports = &viewport;
		viewportInfo.scissorCount = 1;
		viewportInfo.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizerInfo{};
		rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizerInfo.depthClampEnable = VK_FALSE;
		rasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizerInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizerInfo.lineWidth = 1.0f;
		rasterizerInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizerInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizerInfo.depthBiasEnable = VK_FALSE;
		rasterizerInfo.depthBiasConstantFactor = 0.0f;
		rasterizerInfo.depthBiasClamp = 0.0f;
		rasterizerInfo.depthBiasSlopeFactor = 0.0f;

		VkPipelineMultisampleStateCreateInfo multisamplingInfo{};
		multisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisamplingInfo.sampleShadingEnable = VK_FALSE;
		multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisamplingInfo.minSampleShading = 1.0f; // Optional
		multisamplingInfo.pSampleMask = nullptr; // Optional
		multisamplingInfo.alphaToCoverageEnable = VK_FALSE; // Optional
		multisamplingInfo.alphaToOneEnable = VK_FALSE; // Optional

		VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
		depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilInfo.flags;
		depthStencilInfo.depthTestEnable = VK_TRUE;
		depthStencilInfo.depthWriteEnable = VK_TRUE;
		depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		depthStencilInfo.depthBoundsTestEnable;
		depthStencilInfo.stencilTestEnable;
		depthStencilInfo.front;
		depthStencilInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
		depthStencilInfo.minDepthBounds;
		depthStencilInfo.maxDepthBounds;

		std::vector<VkPipelineColorBlendAttachmentState> blendStates(_numColorAttachments);
		for (auto& blendState : blendStates)
		{
			blendState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			blendState.blendEnable = VK_FALSE;
			blendState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
			blendState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
			blendState.colorBlendOp = VK_BLEND_OP_ADD; // Optional
			blendState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
			blendState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
			blendState.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
		}

		VkPipelineColorBlendStateCreateInfo colorBlendingInfo{};
		colorBlendingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendingInfo.logicOpEnable = VK_FALSE;
		colorBlendingInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlendingInfo.attachmentCount = _numColorAttachments;
		colorBlendingInfo.pAttachments = blendStates.data();
		colorBlendingInfo.blendConstants[0] = 0.0f; // Optional
		colorBlendingInfo.blendConstants[1] = 0.0f; // Optional
		colorBlendingInfo.blendConstants[2] = 0.0f; // Optional
		colorBlendingInfo.blendConstants[3] = 0.0f; // Optional

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStageCreateInfos.data();
		pipelineInfo.pVertexInputState = &vertexInputStateInfo;
		pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
		pipelineInfo.pViewportState = &viewportInfo;
		pipelineInfo.pRasterizationState = &rasterizerInfo;
		pipelineInfo.pMultisampleState = &multisamplingInfo;
		pipelineInfo.pDepthStencilState = &depthStencilInfo;
		pipelineInfo.pColorBlendState = &colorBlendingInfo;
		/*std::vector<VkDynamicState> dynamicStates =
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();*/
		//pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = _renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		VkPipeline pipeline{};
		if (vkCreateGraphicsPipelines(_logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create graphics pipeline");
		}

		for (auto& info : shaderStageCreateInfos)
		{
			vkDestroyShaderModule(_logicalDevice, info.module, nullptr);
		}

		vkDestroyPipelineLayout(_logicalDevice, pipelineLayout, nullptr);

		return pipeline;
	}

	VkSurfaceFormatKHR VulkanBootstrap::ConvertFormat(const ImageFormat& _format)
	{
		VkSurfaceFormatKHR format{};

		switch (_format)
		{
		case ImageFormat::ifD32_SFLOAT:
			format.format = VK_FORMAT_D32_SFLOAT;
			break;
		case ImageFormat::ifB8G8R8A8_UNORM:
			format.format = VK_FORMAT_B8G8R8A8_UNORM;
			break;
		}

		format.colorSpace;

		return format;
	}

	VkImageUsageFlagBits VulkanBootstrap::ConvertImageUsage(const ImageUsage& _usage)
	{
		switch (_usage)
		{
		case ImageUsage::iuCOLOR_ATTACHMENT: return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		case ImageUsage::iuDEPTH_STENCIL_ATTACHMENT: return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		}

		return VkImageUsageFlagBits{};
	}

	VkAttachmentLoadOp VulkanBootstrap::ConvertAttachmentLoadOperation(const ImageOperation& _opeartion)
	{
		switch (_opeartion)
		{
		case ImageOperation::ioLOAD: return VK_ATTACHMENT_LOAD_OP_LOAD;
		case ImageOperation::ioCLEAR: return VK_ATTACHMENT_LOAD_OP_CLEAR;
		}

		return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	}

	VkAttachmentStoreOp VulkanBootstrap::ConvertAttachmentStoreOperation(const ImageOperation& _opeartion)
	{
		switch (_opeartion)
		{
		case ImageOperation::ioNONE: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
		case ImageOperation::ioSTORE: return VK_ATTACHMENT_STORE_OP_STORE;
		}

		return VK_ATTACHMENT_STORE_OP_DONT_CARE;
	}

	VkImageLayout VulkanBootstrap::ConvertImageLayout(const ImageLayout& _imageLayout)
	{
		switch (_imageLayout)
		{
		case ImageLayout::ilPRESENT_SOURCE: return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		case ImageLayout::ilDEPTH_STENCIL: return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		case ImageLayout::ilSHADER_READ: return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}

		return VK_IMAGE_LAYOUT_UNDEFINED;
	}
}