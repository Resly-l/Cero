#pragma once
#include "graphics/graphics_api.h"
#include "../thirdparty/vk_bootstrap/VkBootstrap.h"
#include "utility/forward_declaration.h"
#include <unordered_map>

namespace graphics
{
	class VulkanAPI : public GraphicsAPI
	{
	private:
		struct Frame
		{
			VkSemaphore imageAcquiringSemaphore_ = VK_NULL_HANDLE;
			VkSemaphore commandExecutionSemaphore_ = VK_NULL_HANDLE;
			VkFence frameFence_ = VK_NULL_HANDLE;
		};

	private:
		void* windowHandle_ = nullptr;
		vkb::Instance instance_;
		vkb::PhysicalDevice physicalDevice_;
		vkb::Device logicalDevice_;
		vkb::Swapchain swapchain_;
		VkCommandPool commandPool_;
		VkCommandPool transferCommandPool_;
		VkDescriptorPool descriptorPool_;

		std::vector<std::shared_ptr<VulkanRenderTarget>> swapchainRenderTargets_;
		std::vector<Frame> frames_;
		uint32_t swapchainImageIndex_ = 0;
		uint32_t frameIndex_ = 0;

	public:
		VulkanAPI(void* _window);
		~VulkanAPI();

	public:
		VkQueue GetGraphicsQueue() const;

		virtual std::shared_ptr<Pipeline> CreatePipeline(const Pipeline::Layout& _pipelineLayout) override;
		virtual std::shared_ptr<Mesh> CreateMesh(const Mesh::Layout& _meshLayout) override;
		virtual std::shared_ptr<Material> CreateMaterial() override;
		virtual std::shared_ptr<UniformBuffer> CreateUniformBuffer(const UniformBuffer::Layout& _layout) override;
		virtual std::shared_ptr<Texture> CreateTexture(const Texture::Layout& _textureLayout) override;
		virtual std::shared_ptr<RenderPass> CreateRenderPass() override;

		virtual uint32_t GetCurrentFrameIndex() const override;
		virtual std::shared_ptr<RenderTarget> GetSwapchainRenderTarget() const override;
		virtual bool WaitSwapchainImage() override;
		virtual void Present() override;
		virtual void WaitIdle() override;

		VkExtent2D GetSwapchainExtent() const;
		VkSemaphore GetImageAcquiringSemaphore() const;
		VkSemaphore GetCommandExecutionSemaphore() const;
		VkFence GetFrameFence() const;
		VkCommandBuffer AllocateCommnadBuffer() const;
		VkDescriptorSet AllocateDescriptorSet(VkDescriptorSetLayout _descriptorSetLayout) const;

	private:
		void CreateInstance();
		void SelectPhysicalDevice(void* _window);
		void CreateLogicalDevice();
		void CreateSwapchain();
		void CreateCommandPools();
		void CreateDescriptorPool();
		void CreateSyncObjects();
		void CreateSwapchainRenderTargets();
		void RecreateSwapchain();

		VkDescriptorPool CreateDescriptorPool(const std::vector<VkDescriptorSetLayoutBinding>& _bindings) const;
	};
}