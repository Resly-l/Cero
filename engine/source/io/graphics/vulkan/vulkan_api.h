#pragma once
#include "io/graphics/graphics_api.h"
#include "vulkan/vulkan.h"

struct GLFWwindow;

namespace io::graphics
{
	class VulkanPipeline;
	class VulkanRenderPass;

	class VulkanAPI : public GraphicsAPI
	{
	private:
		static constexpr uint32_t frameConcurrency_ = 3;

		VkInstance instance_;
		VkSurfaceKHR surface_;
		VkPhysicalDevice physicalDevice_;
		VkDevice logicalDevice_;
		uint32_t graphicsQueueFamilyIndex_;
		uint32_t presentQueueFamilyIndex_;
		VkQueue graphicsQueue_;
		VkQueue presentQueue_;
		VkCommandPool commandPool_;
		std::vector<VkCommandBuffer> commandBuffers_;
		std::vector<VkSemaphore> imageAvailableSemaphores_;
		std::vector<VkSemaphore> renderFinishedSemaphores_;
		std::vector<VkFence> fences_;
		uint32_t currentFrame_ = 0;
		uint32_t currentImageIndex_ = 0;

		VkSwapchainKHR swapChain_;
		VkSurfaceFormatKHR swapChainFormat_;
		VkExtent2D swapChainExtent_{};
		VkRenderPass swapChainRenderPass_;
		std::vector<VkImageView> swapChainImageViews_;
		std::vector<VkFramebuffer> swapChainFramebuffers_;

		std::unique_ptr<VulkanPipeline> swapChainPipeline_;
		std::shared_ptr<VulkanRenderPass> renderPass_;
		std::shared_ptr<VulkanPipeline> pipeline_;

		// Currently wating semaphore
		VkSemaphore waitingSemaphore_{};
		// Semaphore of last draw call that needs to be wait signal before queueing new one
		VkSemaphore lastSemaphore_{};
		// Semaphores created during multiple render pass draw calls
		// Needs to be destroyed after signaled
		std::vector<std::pair<uint32_t, VkSemaphore>> renderPassSemaphores_;

	public:
		VulkanAPI(GLFWwindow& _window);
		~VulkanAPI();

	public:
		virtual std::shared_ptr<RenderPass> CreateRenderPass(const RenderPassLayout& _renderPassLayout, uint32_t _width, uint32_t _height) override;
		virtual std::shared_ptr<Pipeline> CreatePipeline(const PipelineState& _pipelineState, const RenderPass& _renderPass) override;
		virtual void BindRenderPass(std::shared_ptr<RenderPass> _renderPass) override;
		virtual void BindPipeline(std::shared_ptr<Pipeline> _pipeline) override;

		virtual void BeginFrame() override;
		virtual void Draw() override;
		virtual void EndFrame() override;
	};
}