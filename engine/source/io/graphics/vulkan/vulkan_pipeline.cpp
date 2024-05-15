#include "vulkan_pipeline.h"
#include "vulkan_validation.hpp"
#include "vulkan_utility.h"
#include "io/file/file_interface.h"

namespace io::graphics
{
	VulkanFramebuffer::VulkanFramebuffer(VkDevice _logicalDevice, VkRenderPass _renderPass, const Framebuffer::Layout& _framebufferLayout)
		: logicalDevice_(_logicalDevice)
		, width_(_framebufferLayout.width_)
		, height_(_framebufferLayout.height_)
	{
		InitializeAttachments(_logicalDevice, _framebufferLayout);
		CreateInstance(_logicalDevice, _renderPass);
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		for (auto& attachment : attachments_)
		{
			vkDestroyImageView(logicalDevice_, attachment.imageView_, nullptr);

			if (attachment.image_ == VK_NULL_HANDLE)
			{
				continue;
			}

			vkFreeMemory(logicalDevice_, attachment.memory_, nullptr);
			vkDestroyImage(logicalDevice_, attachment.image_, nullptr);
		}

		vkDestroyFramebuffer(logicalDevice_, instance_, nullptr);
	}

	VkFramebuffer VulkanFramebuffer::GetInstance() const
	{
		return instance_;
	}

	uint32_t VulkanFramebuffer::GetNumAttachements(bool _excludeDepthAttachement) const
	{
		return (uint32_t)attachments_.size();
	}

	void VulkanFramebuffer::InitializeAttachments(VkDevice _logicalDevice,  const Framebuffer::Layout& _framebufferLayout)
	{
		for (const auto& attachementDescription : _framebufferLayout.attachments_)
		{
			VulkanAttachment attachment;
			if (auto imageView = reinterpret_cast<VkImageView>(attachementDescription.attachmentHandle_))
			{
				attachment.imageView_ = imageView;
			}
			else
			{
				VkImageCreateInfo imageCreateInfo{};
				imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
				imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
				imageCreateInfo.format = VulkanTypeConverter::Convert(attachementDescription.format_);
				imageCreateInfo.extent.width = attachementDescription.width_;
				imageCreateInfo.extent.height = attachementDescription.height_;
				imageCreateInfo.extent.depth = 1;
				imageCreateInfo.mipLevels = 1;
				imageCreateInfo.arrayLayers = 1;
				imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
				imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
				imageCreateInfo.usage = VulkanTypeConverter::Convert(attachementDescription.usage_) | VK_IMAGE_USAGE_SAMPLED_BIT;
				vkCreateImage(_logicalDevice, &imageCreateInfo, nullptr, &attachment.image_) >> VulkanResultChecker::GetInstance();

				VkMemoryRequirements memoryRequirements{};
				vkGetImageMemoryRequirements(_logicalDevice, attachment.image_, &memoryRequirements);
				VkMemoryAllocateInfo memoryAllocateInfo{};
				memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				memoryAllocateInfo.allocationSize = memoryRequirements.size;
				//memoryAllocateInfo.memoryTypeIndex = memoryRequirements.memoryTypeBits;
				vkAllocateMemory(_logicalDevice, &memoryAllocateInfo, nullptr, &attachment.memory_);
				vkBindImageMemory(_logicalDevice, attachment.image_, attachment.memory_, 0);

				VkImageViewCreateInfo imageViewCreateInfo{};
				imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				imageViewCreateInfo.format = VulkanTypeConverter::Convert(attachementDescription.format_);
				imageViewCreateInfo.subresourceRange.aspectMask = VulkanTypeConverter::GetAspectMask(imageCreateInfo.format, imageCreateInfo.usage);
				imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
				imageViewCreateInfo.subresourceRange.levelCount = 1;
				imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
				imageViewCreateInfo.subresourceRange.layerCount = 1;
				imageViewCreateInfo.image = attachment.image_;
				vkCreateImageView(_logicalDevice, &imageViewCreateInfo, nullptr, &attachment.imageView_) >> VulkanResultChecker::GetInstance();
			}
			attachments_.push_back(attachment);
		}
	}

	void VulkanFramebuffer::CreateInstance(VkDevice _logicalDevice, VkRenderPass _renderPass)
	{
		std::vector<VkImageView> imageViews;
		for (VulkanAttachment& attachment : attachments_)
		{
			imageViews.push_back(attachment.imageView_);
		}

		VkFramebufferCreateInfo framebufferCreateInfo{};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = _renderPass;
		framebufferCreateInfo.attachmentCount = (uint32_t)attachments_.size();
		framebufferCreateInfo.width = width_;
		framebufferCreateInfo.height = height_;
		framebufferCreateInfo.layers = 1;
		framebufferCreateInfo.pAttachments = imageViews.data();
		vkCreateFramebuffer(_logicalDevice, &framebufferCreateInfo, nullptr, &instance_) >> VulkanResultChecker::GetInstance();
	}

	static VkShaderModule CreateShaderModule(VkDevice _logicalDevice, const std::vector<char>& _shaderCode)
	{
		VkShaderModuleCreateInfo shaderModuleCreateInfo{};
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.codeSize = _shaderCode.size();
		shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(_shaderCode.data());

		VkShaderModule shaderModule;
		vkCreateShaderModule(_logicalDevice, &shaderModuleCreateInfo, nullptr, &shaderModule) >> VulkanResultChecker::GetInstance();

		return shaderModule;
	}

	VulkanPipeline::VulkanPipeline(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, const Pipeline::Layout& _pipelineLayout)
		: logicalDevice_(_logicalDevice)
	{
		LoadShaders(_pipelineLayout.vertexShaderPath, _pipelineLayout.pixelShaderPath);
		CreateInstance(_physicalDevice, _pipelineLayout);
		CreateFramebuffers(_pipelineLayout.frameBufferLayouts);
	}

	VulkanPipeline::~VulkanPipeline()
	{
		framebuffers_.clear();
		vkDestroyRenderPass(logicalDevice_, renderPass_, nullptr);
		vkDestroyPipeline(logicalDevice_, instance_, nullptr);
	}

	VkPipeline VulkanPipeline::GetInstance() const
	{
		return instance_;
	}

	VkRenderPass VulkanPipeline::GetRenderPass() const
	{
		return renderPass_;
	}

	VkFramebuffer VulkanPipeline::GetFramebuffer(uint32_t _index) const
	{
		if (framebuffers_.size() <= _index)
		{
			return VK_NULL_HANDLE;
		}

		return framebuffers_[_index]->GetInstance();
	}

	void VulkanPipeline::LoadShaders(std::wstring_view _vsPath, std::wstring_view _fsPath)
	{
		auto vsCode = file::FileInterface::LoadFile(_vsPath, true, true);
		if (_vsPath.empty() != vsCode.empty())
		{
			throw std::exception("vertex shader path specified but failed to load");
		}

		auto psCode = file::FileInterface::LoadFile(_fsPath, true, true);
		if (_fsPath.empty() != psCode.empty())
		{
			throw std::exception("vertex shader path specified but failed to load");
		}

		vertexShaderModule_ = CreateShaderModule(logicalDevice_, vsCode);
		pixelShaderModule_ = CreateShaderModule(logicalDevice_, psCode);
	}

	void VulkanPipeline::CreateInstance(VkPhysicalDevice _physicalDevice, const Pipeline::Layout& _pipelineLayout)
	{
		VkPipelineShaderStageCreateInfo vertexShaderCreateInfo{};
		vertexShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertexShaderCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertexShaderCreateInfo.module = vertexShaderModule_;
		vertexShaderCreateInfo.pName = "main";
		VkPipelineShaderStageCreateInfo pixelShaderCreateInfo{};
		pixelShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pixelShaderCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		pixelShaderCreateInfo.module = pixelShaderModule_;
		pixelShaderCreateInfo.pName = "main";
		VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShaderCreateInfo, pixelShaderCreateInfo };

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.pVertexBindingDescriptions = nullptr;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.pVertexAttributeDescriptions = nullptr;

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
		inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)_pipelineLayout.viewport_.width_;
		viewport.height = (float)_pipelineLayout.viewport_.height_;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent.width = _pipelineLayout.viewport_.width_;
		scissor.extent.height = _pipelineLayout.viewport_.height_;

		std::vector<VkDynamicState> dynamicStates =
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizerState{};
		rasterizerState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizerState.rasterizerDiscardEnable = VK_FALSE;
		rasterizerState.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizerState.lineWidth = 1.0f;
		rasterizerState.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizerState.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizerState.depthBiasEnable = VK_FALSE;
		rasterizerState.depthClampEnable = VK_FALSE;
		rasterizerState.depthBiasConstantFactor = 0.0f;
		rasterizerState.depthBiasClamp = 0.0f;
		rasterizerState.depthBiasSlopeFactor = 0.0f;

		VkPipelineMultisampleStateCreateInfo multisampleState{};
		multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleState.sampleShadingEnable = VK_FALSE;
		multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleState.minSampleShading = 1.0f;
		multisampleState.pSampleMask = nullptr;
		multisampleState.alphaToCoverageEnable = VK_FALSE;
		multisampleState.alphaToOneEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		/*colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;*/

		VkPipelineColorBlendStateCreateInfo colorBlendState{};
		colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendState.logicOpEnable = VK_FALSE;
		colorBlendState.logicOp = VK_LOGIC_OP_COPY;
		colorBlendState.attachmentCount = 1;
		colorBlendState.pAttachments = &colorBlendAttachment;
		colorBlendState.blendConstants[0] = 0.0f;
		colorBlendState.blendConstants[1] = 0.0f;
		colorBlendState.blendConstants[2] = 0.0f;
		colorBlendState.blendConstants[3] = 0.0f;

		VkPipelineLayout layout;
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = 0;
		pipelineLayoutCreateInfo.pSetLayouts = nullptr;
		pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
		vkCreatePipelineLayout(logicalDevice_, &pipelineLayoutCreateInfo, nullptr, &layout) >> VulkanResultChecker::GetInstance();

		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = VK_FORMAT_B8G8R8A8_UNORM;
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

		VkSubpassDescription subpassDescription{};
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.inputAttachmentCount = 0;
		subpassDescription.pInputAttachments = nullptr;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorAttachmentRef;
		subpassDescription.pDepthStencilAttachment;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassCreateInfo{};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.attachmentCount = 1;
		renderPassCreateInfo.pAttachments = &colorAttachment;
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpassDescription;
		renderPassCreateInfo.dependencyCount = 1;
		renderPassCreateInfo.pDependencies = &dependency;
		vkCreateRenderPass(logicalDevice_, &renderPassCreateInfo, nullptr, &renderPass_) >> VulkanResultChecker::GetInstance();

		VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.stageCount = 2;
		pipelineCreateInfo.pStages = shaderStages;
		pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
		pipelineCreateInfo.pInputAssemblyState = &inputAssemblyInfo;
		pipelineCreateInfo.pViewportState = &viewportState;
		pipelineCreateInfo.pRasterizationState = &rasterizerState;
		pipelineCreateInfo.pMultisampleState = &multisampleState;
		pipelineCreateInfo.pDepthStencilState = nullptr; // transient
		pipelineCreateInfo.pColorBlendState = &colorBlendState;
		pipelineCreateInfo.pDynamicState = &dynamicState;
		pipelineCreateInfo.layout = layout;
		pipelineCreateInfo.renderPass = renderPass_;
		pipelineCreateInfo.subpass = 0;
		pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineCreateInfo.basePipelineIndex = -1;
		vkCreateGraphicsPipelines(logicalDevice_, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &instance_) >> VulkanResultChecker::GetInstance();

		vkDestroyPipelineLayout(logicalDevice_, layout, nullptr);
		vkDestroyShaderModule(logicalDevice_, vertexShaderModule_, nullptr);
		vkDestroyShaderModule(logicalDevice_, pixelShaderModule_, nullptr);
	}

	void VulkanPipeline::CreateFramebuffers(const std::vector<Framebuffer::Layout>& _framebufferLayouts)
	{
		for (auto& layout : _framebufferLayouts)
		{
			framebuffers_.push_back(std::make_unique<VulkanFramebuffer>(logicalDevice_, renderPass_, layout));
		}
	}
}
