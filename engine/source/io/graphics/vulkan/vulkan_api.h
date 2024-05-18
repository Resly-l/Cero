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
		std::vector<std::unique_ptr<VulkanRenderTarget>> swapchainRenderTargets_;
		std::vector<Frame> frames_;
		VkCommandPool commandPool_;
		VkCommandPool transfereCommandPool_;
		uint32_t swapchainImageIndex_ = 0;
		uint32_t frameIndex_ = 0;

		bool pendingSwapchainRecreation_ = false;

		std::shared_ptr<VulkanMesh> testMesh_;

	public:
		VulkanAPI(void* _window);
		~VulkanAPI();

	public:
		virtual std::shared_ptr<Pipeline> CreatePipeline(const Pipeline::Layout& _pipelineLayout) override;
		virtual std::shared_ptr<RenderTarget> CreateRenderTarget(const RenderTarget::Layout& _renderTargetLayout) override;
		virtual std::shared_ptr<Mesh> CreateMesh(const Mesh::Layout& _meshLayout) override;

		virtual void BindPipeline(std::shared_ptr<Pipeline> _pipeline) override;
		virtual void BindRenderTarget(std::shared_ptr<RenderTarget> _renderTarget) override;
		virtual void BindMesh(std::shared_ptr<Mesh> _mesh) override;

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
		void CreateSwapchainRenderTargets();
		void CreateCommandPools();
		void CreateCommandBuffers();
		void CreateSyncObjects();
		bool RecreateSwapchain();
	};
}