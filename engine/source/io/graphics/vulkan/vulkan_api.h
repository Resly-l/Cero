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
			VkCommandBuffer commandBuffer_ = VK_NULL_HANDLE;
			VkSemaphore imageAcquiringSemaphore_ = VK_NULL_HANDLE;
			VkSemaphore commandExecutionSemaphore_ = VK_NULL_HANDLE;
			VkFence frameFence_ = VK_NULL_HANDLE;
			std::map<std::shared_ptr<VulkanPipeline>, VkDescriptorSet> descriptorSets_;
		};

	private:
		void* windowHandle_ = nullptr;
		vkb::Instance instance_;
		vkb::PhysicalDevice physicalDevice_;
		vkb::Device logicalDevice_;
		vkb::Swapchain swapchain_;
		VkCommandPool commandPool_;
		VkCommandPool transfereCommandPool_;
		VkDescriptorPool descriptorPool_;

		std::vector<std::shared_ptr<VulkanRenderTarget>> swapchainRenderTargets_;
		std::vector<Frame> frames_;
		uint32_t swapchainImageIndex_ = 0;
		uint32_t frameIndex_ = 0;

		std::shared_ptr<VulkanPipeline> pipeline_;
		std::shared_ptr<VulkanRenderTarget> renderTarget_;
		std::shared_ptr<VulkanMesh> mesh_;

	public:
		VulkanAPI(void* _window);
		~VulkanAPI();

	public:
		virtual std::shared_ptr<Pipeline> CreatePipeline(const Pipeline::Layout& _pipelineLayout) override;
		virtual std::shared_ptr<Mesh> CreateMesh(const Mesh::Layout& _meshLayout) override;
		virtual std::shared_ptr<RenderTarget> GetSwapchainRenderTarget() override;

		virtual void BindPipeline(std::shared_ptr<Pipeline> _pipeline) override;
		virtual void BindRenderTarget(std::shared_ptr<RenderTarget> _renderTarget) override;
		virtual void BindMesh(std::shared_ptr<Mesh> _mesh) override;

		virtual bool BeginFrame() override;
		virtual void Draw() override;
		virtual void EndFrame() override;

		virtual void Resize(uint32_t _width, uint32_t _height) override {};
		virtual void WaitIdle() override;

	private:
		void CreateInstance();
		void SelectPhysicalDevice(void* _window);
		void CreateLogicalDevice();
		void CreateSwapchain();
		void CreateCommandPools();
		void CreateDescriptorPool();
		void CreateCommandBuffers();
		void CreateSyncObjects();
		void CreateSwapchainRenderTargets();
		void RecreateSwapchain();
	};
}