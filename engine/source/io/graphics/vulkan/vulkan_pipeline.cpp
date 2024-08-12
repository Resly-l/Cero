#include "vulkan_pipeline.h"
#include "vulkan_utility.h"
#include "vulkan_result.hpp"
#include "vulkan_render_target.h"
#include "vulkan_uniform_buffer.h"
#include "vulkan_texture.h"
#include "vulkan_shader_binding.h"
#include "io/file/file_interface.h"

namespace io::graphics
{
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

	VulkanPipeline::VulkanPipeline(VkDevice _logicalDevice, VkPhysicalDevice _physicalDevice, VkDescriptorPool _descriptorPool, const Pipeline::Layout& _pipelineLayout)
		: logicalDevice_(_logicalDevice)
		, physicalDevice_(_physicalDevice)
		, useDepthStencil_(_pipelineLayout.depthFunc_ != ComparisonFunc::NONE)
		, shaderDescriptor_(_pipelineLayout.descriptor_)
		, descriptorPool_(_descriptorPool)
	{
		LoadShaders(_pipelineLayout.vertexShaderPath_, _pipelineLayout.pixelShaderPath_);
		CreateInstance(_physicalDevice, _pipelineLayout);
	}

	VulkanPipeline::~VulkanPipeline()
	{
		shaderBindings_.clear();
		vkDestroyDescriptorSetLayout(logicalDevice_, descriptorSetLayout_, nullptr);
		vkDestroyRenderPass(logicalDevice_, renderPass_, nullptr);
		vkDestroyPipelineLayout(logicalDevice_, layout_, nullptr);
		vkDestroyPipeline(logicalDevice_, instance_, nullptr);
	}

	std::shared_ptr<RenderTarget> VulkanPipeline::CreateRenderTarget(uint32_t _width, uint32_t _height) const
	{
		RenderTarget::Layout renderTargetLayout;
		renderTargetLayout.width_ = _width;
		renderTargetLayout.height_ = _height;
		renderTargetLayout.attachments_ = shaderDescriptor_.outputs;

		return std::make_shared<VulkanRenderTarget>(logicalDevice_, renderTargetLayout);
	}

	VkPipeline VulkanPipeline::GetInstance() const
	{
		return instance_;
	}

	VkPipelineLayout VulkanPipeline::GetLayout() const
	{
		return layout_;
	}

	VkRenderPass VulkanPipeline::GetRenderPass() const
	{
		return renderPass_;
	}

	VkDescriptorSetLayout VulkanPipeline::GetDescriptorSetLayout() const
	{
		return descriptorSetLayout_;
	}

	void VulkanPipeline::UpdateDescriptorSet(VkDescriptorSet _descriptorSet)
	{
		std::vector< VkWriteDescriptorSet> descriptorWrites;
		for (auto& binding : shaderBindings_)
		{
			descriptorWrites.push_back(binding->GetDescriptorWrite(_descriptorSet));
		}
		vkUpdateDescriptorSets(logicalDevice_, (uint32_t)descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
	}

	uint32_t VulkanPipeline::GetNumBindings() const
	{
		return (uint32_t)shaderBindings_.size();
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
		// shader
		VkPipelineShaderStageCreateInfo shaderStages[2] = {};
		{
			VkPipelineShaderStageCreateInfo vertexShaderCreateInfo{};
			vertexShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vertexShaderCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			vertexShaderCreateInfo.module = vertexShaderModule_;
			vertexShaderCreateInfo.pName = "main";
			shaderStages[0] = vertexShaderCreateInfo;

			VkPipelineShaderStageCreateInfo pixelShaderCreateInfo{};
			pixelShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			pixelShaderCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			pixelShaderCreateInfo.module = pixelShaderModule_;
			pixelShaderCreateInfo.pName = "main";
			shaderStages[1] = pixelShaderCreateInfo;
		}

		// input description
		VkVertexInputBindingDescription bindingDescription{};
		std::vector<VkVertexInputAttributeDescription> attributeDesctriptions;
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
		{
			bindingDescription.binding = 0;
			bindingDescription.stride = (uint32_t)_pipelineLayout.vertexInputLayout_.GetSizeInBytes();
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			for (size_t i = 0; i < _pipelineLayout.vertexInputLayout_.GetNumAttibutes(); i++)
			{
				VkVertexInputAttributeDescription attributeDesctription{};
				attributeDesctription.binding = 0;
				attributeDesctription.location = (uint32_t)i;

				switch (_pipelineLayout.vertexInputLayout_.GetAttributeSize(i))
				{
				case 8: attributeDesctription.format = VK_FORMAT_R32G32_SFLOAT; break;
				case 12: attributeDesctription.format = VK_FORMAT_R32G32B32_SFLOAT; break;
				}

				attributeDesctription.offset = (uint32_t)_pipelineLayout.vertexInputLayout_.GetAttributeOffset(i);
				attributeDesctriptions.push_back(attributeDesctription);
			}

			vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputInfo.vertexBindingDescriptionCount = 1;
			vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
			vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)attributeDesctriptions.size();
			vertexInputInfo.pVertexAttributeDescriptions = attributeDesctriptions.data();

			inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssemblyInfo.topology = VulkanTypeConverter::Convert(_pipelineLayout.primitiveTopology_);
			inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
		}

		// dynamic state
		VkViewport viewport{};
		VkRect2D scissor{};
		std::vector<VkDynamicState> dynamicStates;
		VkPipelineViewportStateCreateInfo viewportState{};
		VkPipelineDynamicStateCreateInfo dynamicState{};
		{
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (float)_pipelineLayout.viewport_.width_;
			viewport.height = (float)_pipelineLayout.viewport_.height_;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			scissor.offset = { 0, 0 };
			scissor.extent.width = _pipelineLayout.viewport_.width_;
			scissor.extent.height = _pipelineLayout.viewport_.height_;

			dynamicStates = {
				VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_SCISSOR
			};

			viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportState.viewportCount = 1;
			viewportState.pViewports = &viewport;
			viewportState.scissorCount = 1;
			viewportState.pScissors = &scissor;

			dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
			dynamicState.pDynamicStates = dynamicStates.data();
		}

		//rasterizer
		VkPipelineRasterizationStateCreateInfo rasterizerState{};
		{
			rasterizerState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizerState.rasterizerDiscardEnable = VK_FALSE;
			rasterizerState.polygonMode = VK_POLYGON_MODE_FILL;
			rasterizerState.lineWidth = 1.0f;
			rasterizerState.cullMode = VK_CULL_MODE_NONE;
			rasterizerState.frontFace = VK_FRONT_FACE_CLOCKWISE;
			rasterizerState.depthBiasEnable = VK_FALSE;
			rasterizerState.depthClampEnable = VK_FALSE;
			rasterizerState.depthBiasConstantFactor = 0.0f;
			rasterizerState.depthBiasClamp = 0.0f;
			rasterizerState.depthBiasSlopeFactor = 0.0f;
		}

		// multisample
		VkPipelineMultisampleStateCreateInfo multisampleState{};
		{
			multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampleState.sampleShadingEnable = VK_FALSE;
			multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
			multisampleState.minSampleShading = 1.0f;
			multisampleState.pSampleMask = nullptr;
			multisampleState.alphaToCoverageEnable = VK_FALSE;
			multisampleState.alphaToOneEnable = VK_FALSE;
		}

		// depth stencil
		VkPipelineDepthStencilStateCreateInfo depthStencilState{};
		{
			depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			depthStencilState.depthTestEnable = (_pipelineLayout.depthFunc_ != ComparisonFunc::NONE) ? VK_TRUE : VK_FALSE;
			depthStencilState.depthWriteEnable = VK_TRUE;
			depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
			depthStencilState.depthBoundsTestEnable = VK_FALSE;
			depthStencilState.back.failOp = VK_STENCIL_OP_KEEP;
			depthStencilState.back.passOp = VK_STENCIL_OP_KEEP;
			depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;
			depthStencilState.stencilTestEnable = (_pipelineLayout.stencilFunc_ != ComparisonFunc::NONE) ? VK_TRUE : VK_FALSE;
			depthStencilState.front = depthStencilState.back;
		}

		// color blend
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		VkPipelineColorBlendStateCreateInfo colorBlendState{};
		{
			colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			colorBlendAttachment.blendEnable = VK_FALSE;
			/*colorBlendAttachment.blendEnable = VK_TRUE;
			colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
			colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;*/

			colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlendState.logicOpEnable = VK_FALSE;
			colorBlendState.logicOp = VK_LOGIC_OP_COPY;
			colorBlendState.attachmentCount = 1;
			colorBlendState.pAttachments = &colorBlendAttachment;
			colorBlendState.blendConstants[0] = 0.0f;
			colorBlendState.blendConstants[1] = 0.0f;
			colorBlendState.blendConstants[2] = 0.0f;
			colorBlendState.blendConstants[3] = 0.0f;
		}

		// output attachments
		std::vector<VkAttachmentDescription> attachments;
		{
			for (const ShaderDescriptor::Output& output : shaderDescriptor_.outputs)
			{
				VkAttachmentDescription attachment{};
				attachment.format = VulkanTypeConverter::Convert(output.format_);
				attachment.samples = VK_SAMPLE_COUNT_1_BIT;
				attachment.loadOp = VulkanTypeConverter::ConvertLoadOp(output.loadOp_);
				attachment.storeOp = VulkanTypeConverter::ConvertStoreOp(output.storeOp_);
				attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

				switch (output.usage_)
				{
				case ImageUsage::COLOR_ATTACHMENT:
					attachment.finalLayout =  VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
					break;
				case ImageUsage::DEPTH_STENCIL:
					attachment.finalLayout =  VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
					break;
				}

				attachments.push_back(attachment);
			}
		}

		// render pass
		{
			VkAttachmentReference colorAttachmentRef{};
			colorAttachmentRef.attachment = 0;
			colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			VkAttachmentReference depthAttachmentRef{};
			depthAttachmentRef.attachment = 1;
			depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			VkSubpassDescription subpassDescription{};
			subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpassDescription.inputAttachmentCount = 0;
			subpassDescription.pInputAttachments = nullptr;
			subpassDescription.colorAttachmentCount = 1;
			subpassDescription.pColorAttachments = &colorAttachmentRef;
			subpassDescription.pDepthStencilAttachment = &depthAttachmentRef;

			VkSubpassDependency dependency{};
			// indices, dst must be higher than src to prevent circular dependency
			// 0 refers to this subpass
			// VK_SUBPASS_EXTERNAL refers to the subpass before or after
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			// specify the operations to wait on and the stages in which these operations occur
			dependency.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			dependency.srcAccessMask = 0;
			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			VkRenderPassCreateInfo renderPassCreateInfo{};
			renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassCreateInfo.attachmentCount = (uint32_t)attachments.size();
			renderPassCreateInfo.pAttachments = attachments.data();
			renderPassCreateInfo.subpassCount = 1;
			renderPassCreateInfo.pSubpasses = &subpassDescription;
			renderPassCreateInfo.dependencyCount = 1;
			renderPassCreateInfo.pDependencies = &dependency;
			vkCreateRenderPass(logicalDevice_, &renderPassCreateInfo, nullptr, &renderPass_) >> VulkanResultChecker::GetInstance();
		}

		// descriptor
		{
			std::vector<VkDescriptorSetLayoutBinding> descriptorBindings;
			for (auto& resource : shaderDescriptor_.resources_)
			{
				shaderBindings_.push_back(std::static_pointer_cast<VulkanShaderBinding>(resource->GetShaderBinding()));
				descriptorBindings.push_back(shaderBindings_.back()->GetDescriptorLayout());
			}

			VkDescriptorSetLayoutCreateInfo dslCreateInfo{};
			dslCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			dslCreateInfo.bindingCount = (uint32_t)descriptorBindings.size();
			dslCreateInfo.pBindings = descriptorBindings.data();
			vkCreateDescriptorSetLayout(logicalDevice_, &dslCreateInfo, nullptr, &descriptorSetLayout_) >> VulkanResultChecker::GetInstance();
		}

		// layout
		{
			VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
			pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutCreateInfo.setLayoutCount = 1;
			pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout_;
			pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
			pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
			vkCreatePipelineLayout(logicalDevice_, &pipelineLayoutCreateInfo, nullptr, &layout_) >> VulkanResultChecker::GetInstance();
		}

		// instance
		{
			VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
			pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineCreateInfo.stageCount = 2;
			pipelineCreateInfo.pStages = shaderStages;
			pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
			pipelineCreateInfo.pInputAssemblyState = &inputAssemblyInfo;
			pipelineCreateInfo.pViewportState = &viewportState;
			pipelineCreateInfo.pRasterizationState = &rasterizerState;
			pipelineCreateInfo.pMultisampleState = &multisampleState;
			pipelineCreateInfo.pDepthStencilState = &depthStencilState;
			pipelineCreateInfo.pColorBlendState = &colorBlendState;
			pipelineCreateInfo.pDynamicState = &dynamicState;
			pipelineCreateInfo.layout = layout_;
			pipelineCreateInfo.renderPass = renderPass_;
			pipelineCreateInfo.subpass = 0;
			pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
			pipelineCreateInfo.basePipelineIndex = -1;
			vkCreateGraphicsPipelines(logicalDevice_, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &instance_) >> VulkanResultChecker::GetInstance();
		}
		
		vkDestroyShaderModule(logicalDevice_, vertexShaderModule_, nullptr);
		vkDestroyShaderModule(logicalDevice_, pixelShaderModule_, nullptr);
	}
}
