#pragma once
#include "vulkan_device.h"
#include "vulkan_swapchain.h"
#include "vulkan_pipeline.h"
#include "vulkan_commander.h"
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
		VulkanPipeline* pipeline_;
		VulkanCommander commander_;

		std::vector<std::shared_ptr<Pipeline>> pipelines_;
		uint64_t pipelineReleaseInterval_ = 1;
		uint64_t pipelineReleaseCounter_{};

	public:
		VulkanAPI(GLFWwindow& _window);
		~VulkanAPI();

	public:
		virtual void BeginFrame() override;
		virtual std::shared_ptr<Pipeline> CreatePipeline(const PipelineState& _pipelineState) override;
		virtual void BindPipeline(std::shared_ptr<Pipeline> _pipeline) override;
		virtual void Draw() override;
		virtual void EndFrame() override;

		void ReleaseUnusedPipelines();

	private:
		void CreateVulkanInstance();
		void CreateSurface();
	};
}