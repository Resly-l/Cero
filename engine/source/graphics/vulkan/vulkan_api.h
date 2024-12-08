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
			VkCommandBuffer commandBuffer_ = VK_NULL_HANDLE;
			VkSemaphore imageAcquiringSemaphore_ = VK_NULL_HANDLE;
			VkSemaphore commandExecutionSemaphore_ = VK_NULL_HANDLE;
			VkFence frameFence_ = VK_NULL_HANDLE;

			// need to find a way which does not use map
			std::unordered_map<std::shared_ptr<VulkanPipeline>, VkDescriptorSet> descriptorSets_;
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

		std::shared_ptr<VulkanPipeline> pipeline_;
		std::shared_ptr<VulkanRenderTarget> renderTarget_;

		// these must be managed by proper render system
		std::shared_ptr<VulkanMesh> mesh_;
		std::shared_ptr<VulkanMaterial> material_;
		std::shared_ptr<VulkanTexture> defaultTexture_;
		std::unique_ptr<VulkanMaterial> defaultMaterial_;

	public:
		VulkanAPI(void* _window);
		~VulkanAPI();

	public:
		virtual std::shared_ptr<Pipeline> CreatePipeline(const Pipeline::Layout& _pipelineLayout) override;
		virtual std::shared_ptr<Mesh> CreateMesh(const Mesh::Layout& _meshLayout) override;
		virtual std::shared_ptr<Material> CreateMaterial() override;
		virtual std::shared_ptr<UniformBuffer> CreateUniformBuffer(const UniformBuffer::Layout& _layout) override;
		virtual std::shared_ptr<Texture> CreateTexture(const Texture::Layout& _textureLayout) override;
		virtual std::shared_ptr<RenderTarget> GetSwapchainRenderTarget() override;

		/* pipeline must be bound first before render target */
		virtual void BindPipeline(std::shared_ptr<Pipeline> _pipeline) override;
		virtual void BindRenderTarget(std::shared_ptr<RenderTarget> _renderTarget) override;

		virtual void BindMesh(std::shared_ptr<Mesh> _mesh) override;
		virtual void BindMaterial(std::shared_ptr<Material> _material) override;

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

		VkDescriptorPool CreateDescriptorPool(const std::vector<VkDescriptorSetLayoutBinding>& _bindings) const;
	};
}