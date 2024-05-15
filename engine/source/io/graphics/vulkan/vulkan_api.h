#pragma once
#include "io/graphics/graphics_api.h"
#include "thirdparty/vk_bootstrap/VkBootstrap.h"
#include "utility/forward_declaration.h"

namespace io::graphics
{
	class VulkanAPI : public GraphicsAPI
	{
	private:
		struct Frame
		{
			VkCommandBuffer commandBuffer_;
			VkSemaphore imageAcquiringSemaphore_;
			VkSemaphore commandExecutionSemaphore_;
			VkFence frameFence_;
		};

	private:
		void* windowHandle_ = nullptr;
		vkb::Instance instance_;
		vkb::PhysicalDevice physicalDevice_;
		vkb::Device logicalDevice_;
		vkb::Swapchain swapchain_;

		std::unique_ptr<VulkanPipeline> presentationPipeline_;
		std::vector<Frame> frames_;
		VkCommandPool commandPool_;
		uint32_t swapChainImageIndex_ = 0;
		uint32_t frameIndex_ = 0;

		bool pendingSwapchainCreation_ = false;

	public:
		VulkanAPI(void* _window);
		~VulkanAPI();

	public:
		virtual std::shared_ptr<Pipeline> CreatePipeline(const Pipeline::Layout& _pipelineLayout) override;
		virtual std::shared_ptr<RenderTarget> CreateRenderTarget(const RenderTargetLayout& _renderTargetLayout) override;

		virtual void BindPipeline(std::shared_ptr<Pipeline> _pipeline) override;
		virtual void BindRenderTargets(std::vector<std::shared_ptr<RenderTarget>> _renderTargets) override;

		virtual void BeginFrame() override;
		virtual void Draw() override;
		virtual void EndFrame() override;

		virtual void Resize(uint32_t _width, uint32_t _height) override {};

	private:
		void CreateInstance();
		void SelectPhysicalDevice(void* _window);
		void CreateLogicalDevice();
		void CreateSwapchain();
		void CreatePresentationPipeline();
		void CreateCommandPool();
		void CreateCommandBuffers();
		void CreateSyncObjects();
		bool RecreateSwapchain();
	};
}