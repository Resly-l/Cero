#include "vulkan_render_pass.h"
#include "vulkan_api.h"
#include "vulkan_result.hpp"
#include "vulkan_pipeline.h"
#include "vulkan_render_target.h"
#include "vulkan_mesh.h"
#include "vulkan_texture.h"
#include "vulkan_material.h"
#include "utility/log.h"
#include "../thirdparty/vk_bootstrap/VkBootstrap.h"

using utility::Log;

namespace graphics
{
	void VulkanRenderPass::SetPipeline(std::shared_ptr<Pipeline> _pipeline, GraphicsAPI& _graphicsAPI)
	{
		commandBuffers_.clear();
		descriptorSets_.clear();

		pipeline_ = _pipeline;

		auto& vulkanAPI = (VulkanAPI&)_graphicsAPI;
		auto vulkanPipeline = std::static_pointer_cast<VulkanPipeline>(pipeline_);

		for (uint32_t i = 0; i < _graphicsAPI.GetConfig().numFrameConcurrency_; i++)
		{
			commandBuffers_.push_back(vulkanAPI.AllocateCommnadBuffer());
			descriptorSets_.push_back(vulkanAPI.AllocateDescriptorSet(vulkanPipeline->GetDescriptorSetLayout()));
		}
	}

	void VulkanRenderPass::AddDrawable(Drawable _drawable)
	{
		drawables_.push_back(_drawable);
	}

	void VulkanRenderPass::Execute(GraphicsAPI& _graphicsApi, PassResources& _resources)
	{
		if (!pipeline_)
		{
			std::cout << Log::Format(Log::Category::graphics, Log::Level::warning, "RenderPass::Execute() - pipeline_ was nullptr") << std::endl;
			return;
		}

		uint32_t frameIndex = _graphicsApi.GetCurrentFrameIndex();
		VkCommandBuffer commandBuffer = commandBuffers_[frameIndex];
		VkDescriptorSet descriptorSet = descriptorSets_[frameIndex];

		auto& vulkanAPI = (VulkanAPI&)_graphicsApi;
		auto vulkanPipeline = std::static_pointer_cast<VulkanPipeline>(pipeline_);
		auto vulkanRenderTarget = std::static_pointer_cast<VulkanRenderTarget>(_graphicsApi.GetSwapchainRenderTarget());

		VkExtent2D extent = vulkanAPI.GetSwapchainExtent();

		if (vulkanPipeline->IsPendingDescriptorSetUpdate())
		{
			vulkanPipeline->UpdateDescriptorSet(descriptorSets_);
		}

		VkCommandBufferBeginInfo commandBufferBeginInfo{};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		vkResetCommandBuffer(commandBuffer, VkCommandBufferResetFlags{});
		vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

		VkClearValue clearValues[2]{};
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPass renderPass = vulkanPipeline->GetRenderPass();
		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = renderPass;
		renderPassBeginInfo.framebuffer = vulkanRenderTarget->GetFramebuffer(renderPass);
		renderPassBeginInfo.renderArea.extent = extent;
		renderPassBeginInfo.clearValueCount = 2;
		renderPassBeginInfo.pClearValues = clearValues;
		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(extent.width);
		viewport.height = static_cast<float>(extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = extent;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetInstance());

		for (auto& drawable : drawables_)
		{
			auto vulkanMesh = std::static_pointer_cast<VulkanMesh>(drawable.mesh_);
			auto vulkanMaterial = std::static_pointer_cast<VulkanMaterial>(drawable.material_);

			if (vulkanMaterial->IsCompiled() == false)
			{
				vulkanMaterial->UpdateDescriptorSet();
			}

			VkBuffer vertexBuffers[] = { vulkanMesh->GetVertexBuffer() };
			VkDeviceSize offsets[] = { 0 };

			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(commandBuffer, vulkanMesh->GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

			if (pipeline_->GetNumBindings() > 0)
			{
				VkDescriptorSet descriptorSets[2] =
				{
					vulkanMaterial ? vulkanMaterial->GetDescriptorSet() : VK_NULL_HANDLE,
					descriptorSet
				};

				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetLayout(), 0, 2, descriptorSets, 0, nullptr);
			}

			vkCmdDrawIndexed(commandBuffer, drawable.mesh_->GetNumIndices(), 1, 0, 0, 0);
		}

		vkCmdEndRenderPass(commandBuffer);
		vkEndCommandBuffer(commandBuffer) >> VulkanResultChecker::Get();

		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSemaphore waitSemaphores[] = { vulkanAPI.GetImageAcquiringSemaphore() };
		VkSemaphore signalSemaphores[] = { vulkanAPI.GetCommandExecutionSemaphore() };

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;
		vkQueueSubmit(vulkanAPI.GetLogicalDevice().get_queue(vkb::QueueType::graphics).value(), 1, &submitInfo, vulkanAPI.GetFrameFence()) >> VulkanResultChecker::Get();
	}
}