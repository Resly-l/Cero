#include "vulkan_pipeline.h"
#include "vulkan_device.h"
#include "vulkan_commander.h"
#include "vulkan_swapchain.h"
#include "io/file/file_interface.h"
#include "io/graphics/pipeline_state.h"

namespace io::graphics
{
	VulkanPipeline::VulkanPipeline(const VulkanDevice& _device, const PipelineState& _pipelineState, const VulkanCommander& _commander, const VulkanSwapChain& _swapChain)
		: device_(_device)
	{
		auto pipelineLayout = CreatePipelineLayout();
		auto shaderStageInfos = CreateShaderStageInfos(_pipelineState);
		auto vertexBindingDescription = CreateVertexBindingDesc(_pipelineState);
		auto vertexAttributeDescs = CreateVertexAttributeDescs(_pipelineState);
		auto vertexInputInfo = CreateVertexInputInfo(_pipelineState, vertexBindingDescription, vertexAttributeDescs);
		CreateVertexBuffer(_pipelineState, _commander);
		auto inputAssemblyInfo = CreateInputAssemblyInfo(_pipelineState);
		auto viewport = CreateViewport(_pipelineState);
		auto scissor = CreateScissor(_pipelineState);
		auto viewportInfo = CreateViewportInfo(_pipelineState, viewport, scissor);
		auto rasterizerInfo = CreateRasterizationInfo(_pipelineState);
		auto multisamplingInfo = CreateMultisampleInfo(_pipelineState);
		auto depthStencilInfo = CreateDepthStencilInfo(_pipelineState);
		auto colorAttachmentInfo = CreateColorBlendingAttachmentInfo(_pipelineState);
		auto colorBlendingInfo = CreateColorBlendInfo(_pipelineState, colorAttachmentInfo);

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStageInfos.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
		pipelineInfo.pViewportState = &viewportInfo;
		pipelineInfo.pRasterizationState = &rasterizerInfo;
		pipelineInfo.pMultisampleState = &multisamplingInfo;
		//pipelineInfo.pDepthStencilState = &depthStencilInfo; // Optional
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
		pipelineInfo.renderPass = _swapChain.GetRenderPass();
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		if (vkCreateGraphicsPipelines(device_.GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &instance_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create graphics pipeline");
		}

		for (auto& info : shaderStageInfos)
		{
			vkDestroyShaderModule(device_.GetLogicalDevice(), info.module, nullptr);
		}

		vkDestroyPipelineLayout(device_.GetLogicalDevice(), pipelineLayout, nullptr);
	}

	VulkanPipeline::~VulkanPipeline()
	{
		vkDestroyPipeline(device_.GetLogicalDevice(), instance_, nullptr);
		vkDestroyBuffer(device_.GetLogicalDevice(), vertexBuffer_, nullptr);
		vkFreeMemory(device_.GetLogicalDevice(), vertexBufferMemory_, nullptr);
	}

	VkPipeline VulkanPipeline::GetInstance() const
	{
		return instance_;
	}

	VkBuffer VulkanPipeline::GetVertexBuffer() const
	{
		return vertexBuffer_;
	}

	VkPipelineLayout VulkanPipeline::CreatePipelineLayout()
	{
		VkPipelineLayout pipelineLayout;
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0; // Optional
		pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

		if (vkCreatePipelineLayout(device_.GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout");
		}

		return pipelineLayout;
	}

	std::vector<VkPipelineShaderStageCreateInfo> VulkanPipeline::CreateShaderStageInfos(const PipelineState& _pipelineState)
	{
		std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
		auto vertShaderCode = io::file::FileInterface::LoadFile(_pipelineState.vertexShaderPath, true, true);
		auto fragShaderCode = io::file::FileInterface::LoadFile(_pipelineState.pixelShaderPath, true, true);

		VkShaderModule vertexShaderModule, fragShaderModule;
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = vertShaderCode.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(vertShaderCode.data());

		if (vkCreateShaderModule(device_.GetLogicalDevice(), &createInfo, nullptr, &vertexShaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create vertex shader");
		}

		createInfo.codeSize = fragShaderCode.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(fragShaderCode.data());

		if (vkCreateShaderModule(device_.GetLogicalDevice(), &createInfo, nullptr, &fragShaderModule) != VK_SUCCESS)
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

		return shaderStageCreateInfos;
	}

	VkVertexInputBindingDescription VulkanPipeline::CreateVertexBindingDesc(const PipelineState& _pipelineState)
	{
		VkVertexInputBindingDescription vertexInputBindingDescription{};
		vertexInputBindingDescription.binding = 0;
		vertexInputBindingDescription.stride = (uint32_t)_pipelineState.vertexBuffer.GetElementSize();
		vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return vertexInputBindingDescription;
	}

	std::vector<VkVertexInputAttributeDescription> VulkanPipeline::CreateVertexAttributeDescs(const PipelineState& _pipelineState)
	{
		std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;
		VkPipelineVertexInputStateCreateInfo vertexInputStateInfo;
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

		return vertexAttributeDescriptions;
	}

	VkPipelineVertexInputStateCreateInfo VulkanPipeline::CreateVertexInputInfo(const PipelineState& _pipelineState, VkVertexInputBindingDescription& _bindingDescription, std::vector<VkVertexInputAttributeDescription>& _attibuteDescs)
	{
		VkPipelineVertexInputStateCreateInfo vertexInputStateInfo{};
		vertexInputStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputStateInfo.vertexBindingDescriptionCount = 1;
		vertexInputStateInfo.pVertexBindingDescriptions = &_bindingDescription;
		vertexInputStateInfo.vertexAttributeDescriptionCount = (uint32_t)_attibuteDescs.size();
		vertexInputStateInfo.pVertexAttributeDescriptions = _attibuteDescs.data();

		return vertexInputStateInfo;
	}

	void VulkanPipeline::CreateVertexBuffer(const PipelineState& _pipelineState, const VulkanCommander& _commander)
	{
		const size_t bufferSize = _pipelineState.vertexBuffer.GetSizeInBytes();
		auto stagingBuffer = _commander.CreateBuffer(device_, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
		auto stagingBufferMemory = _commander.AllocateMemory(device_, stagingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		void* data;
		vkMapMemory(device_.GetLogicalDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, _pipelineState.vertexBuffer.GetRawBufferAddress(), (size_t)bufferSize);
		vkUnmapMemory(device_.GetLogicalDevice(), stagingBufferMemory);

		vertexBuffer_ = _commander.CreateBuffer(device_, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
		vertexBufferMemory_ = _commander.AllocateMemory(device_, vertexBuffer_, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        _commander.CopyBuffer(device_, stagingBuffer, vertexBuffer_, bufferSize);

        vkDestroyBuffer(device_.GetLogicalDevice(), stagingBuffer, nullptr);
        vkFreeMemory(device_.GetLogicalDevice(), stagingBufferMemory, nullptr);
	}

	VkPipelineInputAssemblyStateCreateInfo VulkanPipeline::CreateInputAssemblyInfo(const PipelineState& _pipelineState)
	{
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
		inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

		return inputAssemblyInfo;
	}

	VkViewport VulkanPipeline::CreateViewport(const PipelineState& _pipelineState)
	{
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)_pipelineState.viewport_.width_;
		viewport.height = (float)_pipelineState.viewport_.height_;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		return viewport;
	}

	VkRect2D VulkanPipeline::CreateScissor(const PipelineState& _pipelineState)
	{
		VkRect2D scissor;
		scissor.offset = { 0, 0 };
		scissor.extent = { _pipelineState.viewport_.width_, _pipelineState.viewport_.height_ };

		return scissor;
	}

	VkPipelineViewportStateCreateInfo VulkanPipeline::CreateViewportInfo(const PipelineState& _pipelineState, VkViewport& _viewport, VkRect2D& _scissor)
	{
		VkPipelineViewportStateCreateInfo viewportInfo{};
		viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportInfo.viewportCount = 1;
		viewportInfo.pViewports = &_viewport;
		viewportInfo.scissorCount = 1;
		viewportInfo.pScissors = &_scissor;

		return viewportInfo;
	}

	VkPipelineRasterizationStateCreateInfo VulkanPipeline::CreateRasterizationInfo(const PipelineState& _pipelineState)
	{
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

		return rasterizerInfo;
	}

	VkPipelineMultisampleStateCreateInfo VulkanPipeline::CreateMultisampleInfo(const PipelineState& _pipelineState)
	{
		VkPipelineMultisampleStateCreateInfo multisamplingInfo{};
		multisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisamplingInfo.sampleShadingEnable = VK_FALSE;
		multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisamplingInfo.minSampleShading = 1.0f; // Optional
		multisamplingInfo.pSampleMask = nullptr; // Optional
		multisamplingInfo.alphaToCoverageEnable = VK_FALSE; // Optional
		multisamplingInfo.alphaToOneEnable = VK_FALSE; // Optional

		return multisamplingInfo;
	}

	VkPipelineDepthStencilStateCreateInfo VulkanPipeline::CreateDepthStencilInfo(const PipelineState& _pipelineState)
	{
		return VkPipelineDepthStencilStateCreateInfo();
	}

	VkPipelineColorBlendAttachmentState VulkanPipeline::CreateColorBlendingAttachmentInfo(const PipelineState& _pipelineState)
	{
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

		return colorBlendAttachment;
	}

	VkPipelineColorBlendStateCreateInfo VulkanPipeline::CreateColorBlendInfo(const PipelineState& _pipelineState, VkPipelineColorBlendAttachmentState& _colorBlendAttachment)
	{
		VkPipelineColorBlendStateCreateInfo colorBlendingInfo{};
		colorBlendingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendingInfo.logicOpEnable = VK_FALSE;
		colorBlendingInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlendingInfo.attachmentCount = 1;
		colorBlendingInfo.pAttachments = &_colorBlendAttachment;
		colorBlendingInfo.blendConstants[0] = 0.0f; // Optional
		colorBlendingInfo.blendConstants[1] = 0.0f; // Optional
		colorBlendingInfo.blendConstants[2] = 0.0f; // Optional
		colorBlendingInfo.blendConstants[3] = 0.0f; // Optional

		return colorBlendingInfo;
	}
}