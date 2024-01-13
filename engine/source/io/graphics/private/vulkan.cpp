#include "vulkan.h"
#include "thirdparty/glfw/glfw3.h"
#include "io/file/file_interface.h"
#include <stdexcept>
#include <iostream>

namespace io::graphics
{
	Vulkan::Pipeline::Pipeline(const Device& _device, const PipelineState& _pipelineState, VkRenderPass _renderPass)
		: GraphicsPipeline(_device, _pipelineState)
	{
		CreatePipelineLayout(_device);

		/*std::vector<VkDynamicState> dynamicStates =
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();*/

		CreateShaderStageInfos(_device, _pipelineState);
		CreateVertexInputCreateInfo(_pipelineState);
		CreateVertexBuffer(_device, _pipelineState);
		CreateInputAssemblyCreateInfo(_pipelineState);
		CreateViewportCreateInfo(_pipelineState);
		CreateRasterizationCreateInfo(_pipelineState);
		CreateMultisampleCreateInfo(_pipelineState);
		CreateDepthStencilCreateInfo(_pipelineState);
		CreateColorBlendCreateInfo(_pipelineState);

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStageInfos_.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo_;
		pipelineInfo.pInputAssemblyState = &inputAssemblyInfo_;
		pipelineInfo.pViewportState = &viewportInfo_;
		pipelineInfo.pRasterizationState = &rasterizerInfo_;
		pipelineInfo.pMultisampleState = &multisamplingInfo_;
		pipelineInfo.pDepthStencilState = nullptr; // Optional
		pipelineInfo.pColorBlendState = &colorBlendingInfo_;
		//pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = pipelineLayout_;
		pipelineInfo.renderPass = _renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		if (vkCreateGraphicsPipelines(_device.logicalDevice_, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &instance_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create graphics pipeline");
		}
	}

	void Vulkan::Pipeline::Release(const Device& _device)
	{
		vkDestroyPipeline(_device.logicalDevice_, instance_, nullptr);
		vkDestroyPipelineLayout(_device.logicalDevice_, pipelineLayout_, nullptr);

		vkDestroyBuffer(_device.logicalDevice_, vertexBuffer_, nullptr);
		vkFreeMemory(_device.logicalDevice_, vertexBufferMemory_, nullptr);

		for (auto& shaderStageInfo : shaderStageInfos_)
		{
			vkDestroyShaderModule(_device.logicalDevice_, shaderStageInfo.module, nullptr);
		}
	}

	void Vulkan::Pipeline::CreatePipelineLayout(const Device& _device)
	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0; // Optional
		pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

		if (vkCreatePipelineLayout(_device.logicalDevice_, &pipelineLayoutInfo, nullptr, &pipelineLayout_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout");
		}
	}

	void Vulkan::Pipeline::CreateShaderStageInfos(const Device& _device, const PipelineState& _pipelineState)
	{
		std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
		auto vertShaderCode = io::file::FileInterface::LoadFile(_pipelineState.vertexShaderPath, true, true);
		auto fragShaderCode = io::file::FileInterface::LoadFile(_pipelineState.pixelShaderPath, true, true);

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = vertShaderCode.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(vertShaderCode.data());

		if (vkCreateShaderModule(static_cast<Device>(_device).logicalDevice_, &createInfo, nullptr, &vertexShaderModule_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create vertex shader");
		}

		createInfo.codeSize = fragShaderCode.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(fragShaderCode.data());

		if (vkCreateShaderModule(static_cast<Device>(_device).logicalDevice_, &createInfo, nullptr, &fragShaderModule_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create fragment shader");
		}

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertexShaderModule_;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule_;
		fragShaderStageInfo.pName = "main";

		shaderStageInfos_.push_back(vertShaderStageInfo);
		shaderStageInfos_.push_back(fragShaderStageInfo);
	}

	void Vulkan::Pipeline::CreateVertexInputCreateInfo(const PipelineState& _pipelineState)
	{
		vertexBindingDescription_.binding = 0;
		vertexBindingDescription_.stride = _pipelineState.vertexBuffer.GetElementSize();
		vertexBindingDescription_.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

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

			vertexAttributeDescription.offset = _pipelineState.vertexBuffer.GetLayout()->GetAttributeOffset(i);

			vertexAttributeDescriptions_.push_back(vertexAttributeDescription);
		}

		vertexInputInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo_.vertexBindingDescriptionCount = 1;
		vertexInputInfo_.pVertexBindingDescriptions = &vertexBindingDescription_;
		vertexInputInfo_.vertexAttributeDescriptionCount = (uint32_t)vertexAttributeDescriptions_.size();
		vertexInputInfo_.pVertexAttributeDescriptions = vertexAttributeDescriptions_.data();
	}

	void Vulkan::Pipeline::CreateVertexBuffer(const Device& _device, const PipelineState& _pipelineState)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = _pipelineState.vertexBuffer.GetSizeInBytes();
		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(_device.logicalDevice_, &bufferInfo, nullptr, &vertexBuffer_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create vertex buffer");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(_device.logicalDevice_, vertexBuffer_, &memRequirements);

		auto FindMemoryType = [&](uint32_t _typeFilter, VkMemoryPropertyFlags _flags)
		{
			VkPhysicalDeviceMemoryProperties memProperties;
			vkGetPhysicalDeviceMemoryProperties(_device.physicalDevice_, &memProperties);

			for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
			{
				if ((_typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & _flags) == _flags)
				{
					return i;
				}
			}

			throw std::runtime_error("failed to find suitable memory type!");
		};

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;

		const VkMemoryPropertyFlags flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, flags);

		if (vkAllocateMemory(_device.logicalDevice_, &allocInfo, nullptr, &vertexBufferMemory_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate vertex buffer memory!");
		}

		vkBindBufferMemory(_device.logicalDevice_, vertexBuffer_, vertexBufferMemory_, 0);

		void* data = nullptr;
		vkMapMemory(_device.logicalDevice_, vertexBufferMemory_, 0, bufferInfo.size, 0, &data);
		memcpy(data, _pipelineState.vertexBuffer.GetRawBufferAddress(), bufferInfo.size);
		vkUnmapMemory(_device.logicalDevice_, vertexBufferMemory_);
	}

	void Vulkan::Pipeline::CreateInputAssemblyCreateInfo(const PipelineState& _pipelineState)
	{
		inputAssemblyInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyInfo_.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyInfo_.primitiveRestartEnable = VK_FALSE;
	}

	void Vulkan::Pipeline::CreateViewportCreateInfo(const PipelineState& _pipelineState)
	{
		viewport_.x = 0.0f;
		viewport_.y = 0.0f;
		viewport_.width = (float)_pipelineState.viewport_.width_;
		viewport_.height = (float)_pipelineState.viewport_.height_;
		viewport_.minDepth = 0.0f;
		viewport_.maxDepth = 1.0f;

		scissor_.offset = { 0, 0 };
		scissor_.extent = { _pipelineState.viewport_.width_, _pipelineState.viewport_.height_ };

		viewportInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportInfo_.viewportCount = 1;
		viewportInfo_.pViewports = &viewport_;
		viewportInfo_.scissorCount = 1;
		viewportInfo_.pScissors = &scissor_;
	}

	void Vulkan::Pipeline::CreateRasterizationCreateInfo(const PipelineState& _pipelineState)
	{
		rasterizerInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizerInfo_.depthClampEnable = VK_FALSE;
		rasterizerInfo_.rasterizerDiscardEnable = VK_FALSE;
		rasterizerInfo_.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizerInfo_.lineWidth = 1.0f;
		rasterizerInfo_.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizerInfo_.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizerInfo_.depthBiasEnable = VK_FALSE;
		rasterizerInfo_.depthBiasConstantFactor = 0.0f;
		rasterizerInfo_.depthBiasClamp = 0.0f;
		rasterizerInfo_.depthBiasSlopeFactor = 0.0f;
	}

	void Vulkan::Pipeline::CreateMultisampleCreateInfo(const PipelineState& _pipelineState)
	{
		multisamplingInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisamplingInfo_.sampleShadingEnable = VK_FALSE;
		multisamplingInfo_.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisamplingInfo_.minSampleShading = 1.0f; // Optional
		multisamplingInfo_.pSampleMask = nullptr; // Optional
		multisamplingInfo_.alphaToCoverageEnable = VK_FALSE; // Optional
		multisamplingInfo_.alphaToOneEnable = VK_FALSE; // Optional
	}

	void Vulkan::Pipeline::CreateDepthStencilCreateInfo(const PipelineState& _pipelineState)
	{
	}

	void Vulkan::Pipeline::CreateColorBlendCreateInfo(const PipelineState& _pipelineState)
	{
		colorBlendAttachment_.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment_.blendEnable = VK_FALSE;
		colorBlendAttachment_.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment_.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment_.colorBlendOp = VK_BLEND_OP_ADD; // Optional
		colorBlendAttachment_.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment_.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment_.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

		colorBlendingInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendingInfo_.logicOpEnable = VK_FALSE;
		colorBlendingInfo_.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlendingInfo_.attachmentCount = 1;
		colorBlendingInfo_.pAttachments = &colorBlendAttachment_;
		colorBlendingInfo_.blendConstants[0] = 0.0f; // Optional
		colorBlendingInfo_.blendConstants[1] = 0.0f; // Optional
		colorBlendingInfo_.blendConstants[2] = 0.0f; // Optional
		colorBlendingInfo_.blendConstants[3] = 0.0f; // Optional
	}

	VkPipeline Vulkan::Pipeline::GetInstance() const
	{
		return instance_;
	}

	VkBuffer Vulkan::Pipeline::GetVertexBuffer() const
	{
		return vertexBuffer_;
	}

	Vulkan::Vulkan(GLFWwindow& _window)
		: window_(_window)
	{
		device_ = std::make_unique<Device>();

		RetrieveFrameBufferSize();

		CreateVulkanInstance();
		CreateSurface();
		SelectPhysicalDevice();
		FindQueueFamilyIndex();
		CreateLogicalDevice();
		CreateSwapChain();
		InitializeSwapChainImages();
		CreateRenderPass();
		CreateFrameBuffer();
		CreateCommandPool();
		CreateCommandBuffer();
		CreateSyncObjects();
	}

	Vulkan::~Vulkan()
	{
		if (GetDevice<Device>() == nullptr)
		{
			return;
		}
		
		GetPipeline<Pipeline>()->Release(*GetDevice<Device>());

		for (size_t i = 0; i < frameConcurrency; i++)
		{
			vkDestroySemaphore(GetDevice<Device>()->logicalDevice_, imageAvailableSemaphores_[i], nullptr);
			vkDestroySemaphore(GetDevice<Device>()->logicalDevice_, renderFinishedSemaphores_[i], nullptr);
			vkDestroyFence(GetDevice<Device>()->logicalDevice_, inFlightFences_[i], nullptr);
		}

		vkDestroyCommandPool(GetDevice<Device>()->logicalDevice_, commandPool_, nullptr);
		vkDestroyRenderPass(GetDevice<Device>()->logicalDevice_, renderPass_, nullptr);

		for (auto framebuffer : swapChainFramebuffers_)
		{
			vkDestroyFramebuffer(GetDevice<Device>()->logicalDevice_, framebuffer, nullptr);
		}

		for (auto imageView : swapChainImageViews_)
		{
			vkDestroyImageView(GetDevice<Device>()->logicalDevice_, imageView, nullptr);
		}

		vkDestroySwapchainKHR(GetDevice<Device>()->logicalDevice_, swapChain_, nullptr);
		vkDestroyDevice(GetDevice<Device>()->logicalDevice_, nullptr);
		vkDestroySurfaceKHR(instance_, surface_, nullptr);
		vkDestroyInstance(instance_, nullptr);
	}

	void Vulkan::BindPipeline(const PipelineState& _pipelineState)
	{
		pipeline_ = std::make_unique<Pipeline>(*GetDevice<Device>(), _pipelineState, renderPass_);
	}

	void Vulkan::Execute() const
	{
		vkWaitForFences(GetDevice<Device>()->logicalDevice_, 1, &inFlightFences_[currentFrame_], VK_TRUE, UINT64_MAX);
		vkResetFences(GetDevice<Device>()->logicalDevice_, 1, &inFlightFences_[currentFrame_]);

		uint32_t imageIndex = 0;
		if (vkAcquireNextImageKHR(GetDevice<Device>()->logicalDevice_, swapChain_, UINT64_MAX, imageAvailableSemaphores_[currentFrame_], VK_NULL_HANDLE, &imageIndex) != VK_SUCCESS)
		{
			throw std::exception("failed to get avilable swap chain index");
		}

		vkResetCommandBuffer(commandBuffers_[currentFrame_], 0);
		RecordCommandBuffer(imageIndex);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { imageAvailableSemaphores_[currentFrame_] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers_[currentFrame_];
		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores_[currentFrame_] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(graphicsQueue_, 1, &submitInfo, inFlightFences_[currentFrame_]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to submit draw command buffer");
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { swapChain_ };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr; // Optional

		vkQueuePresentKHR(presentationQueue_, &presentInfo);

		currentFrame_ = (currentFrame_ + 1) % frameConcurrency;
	}

	void Vulkan::RetrieveFrameBufferSize()
	{
		int width = 0;
		int height = 0;

		glfwGetFramebufferSize(&window_, &width, &height);

		width_ = width;
		height_ = height;
	}

	void Vulkan::CreateVulkanInstance()
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;
#if _DEBUG
		createInfo.enabledLayerCount = validationLayers.size();
		createInfo.ppEnabledLayerNames = validationLayers.data();
#endif

		if (vkCreateInstance(&createInfo, nullptr, &instance_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create instance!");
		}
	}

	void Vulkan::CreateSurface()
	{
		if (glfwCreateWindowSurface(instance_, &window_, nullptr, &surface_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface!");
		}
	}

	void Vulkan::SelectPhysicalDevice()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance_, &deviceCount, devices.data());

		for (const auto& device : devices)
		{
			if (IsDeviceSuitable(device))
			{
				GetDevice<Device>()->physicalDevice_ = device;
				return;
			}
		}

		if (GetDevice<Device>()->physicalDevice_ == nullptr)
		{
			throw std::runtime_error("failed to find GPU with Vulkan support");
		}
	}

	void Vulkan::CreateLogicalDevice()
	{
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> queueFamilyIndices = { *graphicsFamilyIndex_, *presentFamilyIndex_ };

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

		VkPhysicalDeviceFeatures deviceFeatures{};
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = (uint32_t)requiredExtensionNames.size();
		createInfo.ppEnabledExtensionNames = requiredExtensionNames.data();
		createInfo.enabledLayerCount = 0;

		if (vkCreateDevice(GetDevice<Device>()->physicalDevice_, &createInfo, nullptr, &GetDevice<Device>()->logicalDevice_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create logical device!");
		}

		vkGetDeviceQueue(GetDevice<Device>()->logicalDevice_, *graphicsFamilyIndex_, 0, &graphicsQueue_);
		vkGetDeviceQueue(GetDevice<Device>()->logicalDevice_, *presentFamilyIndex_, 0, &presentationQueue_);
	}

	void Vulkan::CreateSwapChain()
	{
		VkSurfaceCapabilitiesKHR capabilities{};
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(GetDevice<Device>()->physicalDevice_, surface_, &capabilities);

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface_;
		createInfo.minImageCount = capabilities.minImageCount + 1;
		createInfo.imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
		createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		createInfo.imageExtent = { width_, height_ };
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		uint32_t queueFamilyIndices[] = { *graphicsFamilyIndex_, *presentFamilyIndex_ };

		if (graphicsFamilyIndex_ != presentFamilyIndex_)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // optional
			createInfo.pQueueFamilyIndices = nullptr; // optional
		}
		createInfo.preTransform = capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(GetDevice<Device>()->logicalDevice_, &createInfo, nullptr, &swapChain_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create swap chain");
		}
	}

	void Vulkan::CreateRenderPass()
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = VK_FORMAT_B8G8R8A8_SRGB;
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

		if (vkCreateRenderPass(GetDevice<Device>()->logicalDevice_, &renderPassInfo, nullptr, &renderPass_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create render pass");
		}
	}

	void Vulkan::CreateFrameBuffer()
	{
		swapChainFramebuffers_.resize(swapChainImageViews_.size());

		for (size_t i = 0; i < swapChainImageViews_.size(); i++)
		{
			VkImageView attachments[] =
			{
				swapChainImageViews_[i]
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass_;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = width_;
			framebufferInfo.height = height_;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(GetDevice<Device>()->logicalDevice_, &framebufferInfo, nullptr, &swapChainFramebuffers_[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create framebuffer");
			}
		}
	}

	void Vulkan::CreateCommandPool()
	{
		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = *graphicsFamilyIndex_;

		if (vkCreateCommandPool(GetDevice<Device>()->logicalDevice_, &poolInfo, nullptr, &commandPool_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create command pool!");
		}
	}

	void Vulkan::CreateCommandBuffer()
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool_;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = frameConcurrency;

		if (vkAllocateCommandBuffers(GetDevice<Device>()->logicalDevice_, &allocInfo, commandBuffers_.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffers");
		}
	}

	void Vulkan::CreateSyncObjects()
	{
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < frameConcurrency; i++)
		{
			if (vkCreateSemaphore(GetDevice<Device>()->logicalDevice_, &semaphoreInfo, nullptr, &imageAvailableSemaphores_[i]) != VK_SUCCESS
				|| vkCreateSemaphore(GetDevice<Device>()->logicalDevice_, &semaphoreInfo, nullptr, &renderFinishedSemaphores_[i]) != VK_SUCCESS
				|| vkCreateFence(GetDevice<Device>()->logicalDevice_, &fenceInfo, nullptr, &inFlightFences_[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create sync objects");
			}
		}
	}

	void Vulkan::InitializeSwapChainImages()
	{
		uint32_t imageCount = 0;
		vkGetSwapchainImagesKHR(GetDevice<Device>()->logicalDevice_, swapChain_, &imageCount, nullptr);
		swapChainImages_.resize(imageCount);
		if (vkGetSwapchainImagesKHR(GetDevice<Device>()->logicalDevice_, swapChain_, &imageCount, swapChainImages_.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to get swap chain images");
		}

		swapChainImageViews_.resize(swapChainImages_.size());

		for (size_t i = 0; i < swapChainImages_.size(); i++)
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = swapChainImages_[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = VK_FORMAT_B8G8R8A8_SRGB;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(GetDevice<Device>()->logicalDevice_, &createInfo, nullptr, &swapChainImageViews_[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create image views");
			}
		}
	}

	void Vulkan::FindQueueFamilyIndex()
	{
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(GetDevice<Device>()->physicalDevice_, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(GetDevice<Device>()->physicalDevice_, &queueFamilyCount, queueFamilies.data());

		for (uint32_t i = 0; i < queueFamilyCount; i++)
		{
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(GetDevice<Device>()->physicalDevice_, i, surface_, &presentSupport);

			if (presentSupport && !presentFamilyIndex_)
			{
				presentFamilyIndex_ = i;
			}

			if ((queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && !graphicsFamilyIndex_)
			{
				graphicsFamilyIndex_ = i;
			}
		}

		if (!graphicsFamilyIndex_ || !presentFamilyIndex_)
		{
			throw std::runtime_error("failed to find queue families");
		}
	}

	void Vulkan::RecordCommandBuffer(const size_t _bufferIndex) const
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(commandBuffers_[currentFrame_], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass_;
		renderPassInfo.framebuffer = swapChainFramebuffers_[_bufferIndex];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = { width_, height_ };
		VkClearValue clearColor{ 0.01f, 0.01f, 0.01f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffers_[currentFrame_], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffers_[currentFrame_], VK_PIPELINE_BIND_POINT_GRAPHICS, GetPipeline<Pipeline>()->GetInstance());

		VkBuffer vertexBuffers[] = {GetPipeline<Pipeline>()->GetVertexBuffer()};
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(commandBuffers_[currentFrame_], 0, 1, vertexBuffers, offsets);

		vkCmdDraw(commandBuffers_[currentFrame_], 3, 1, 0, 0);
		vkCmdEndRenderPass(commandBuffers_[currentFrame_]);

		if (vkEndCommandBuffer(commandBuffers_[currentFrame_]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}
	}

	bool Vulkan::IsDeviceSuitable(VkPhysicalDevice _device)
	{
		// check device type
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(_device, &deviceProperties);

		if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			return false;
		}

		// check required extensions
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(_device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(_device, nullptr, &extensionCount, availableExtensions.data());

		for (const std::string extensionName : requiredExtensionNames)
		{
			if (std::find_if(availableExtensions.begin(), availableExtensions.end(),
				[extensionName](const VkExtensionProperties& extension)
				{
					return extensionName == extension.extensionName;
				}) == availableExtensions.end())
			{
				return false;
			};
		}

		return true;
	}
}