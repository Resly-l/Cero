#pragma once
#include "vulkan_device.h"
#include "vulkan_swapchain.h"
#include "vulkan_pipeline.h"
#include "thirdparty/glfw/glfw3.h"
#include "io/graphics/graphics_api.h"

namespace io::graphics
{
	class VulkanAPI : public GraphicsAPI
	{
	private:
		GLFWwindow& window_;
		VkSurfaceKHR surface_;
		VkInstance instance_;

		VulkanDevice device_;
		VulkanSwapChain swapChain_;

		VulkanPipeline* pipeline_ = nullptr;
		std::vector<std::shared_ptr<DeviceResource>> deviceResources_;

	public:
		VulkanAPI(GLFWwindow& _window);
		~VulkanAPI();

	public:
		virtual std::shared_ptr<Pipeline> CreatePipeline(const PipelineState& _pipelineState) override;
		virtual std::shared_ptr<RenderTarget> CreateRenderTarget(const PipelineState& _pipelineState) override;

		virtual void BindPipeline(std::shared_ptr<Pipeline> _pipeline) override;
		virtual void BindRenderTarget(std::shared_ptr<RenderTarget> _renderTarget) override;

		virtual void BeginFrame() override;
		virtual void Draw() override;
		virtual void EndFrame() override;

	private:
		void ReleaseUnusedPipelines();
	};
}