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
		VulkanPipeline pipeline_;
		VulkanCommander commander_;

	public:
		VulkanAPI(GLFWwindow& _window);
		~VulkanAPI();

	public:
		virtual void BindPipeline(const PipelineState& _pipelineState) override;
		virtual void Draw() override;
		virtual void Present() override;

	private:
		void CreateVulkanInstance();
		void CreateSurface();
	};
}