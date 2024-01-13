#pragma once
#include "utility/stl.h"

#include <vulkan/vulkan.h>
#include "thirdparty/glfw/glfw3.h"

#include "io/graphics/graphics_api.h"

namespace io::graphics
{
	class Vulkan final : public GraphicsAPI
	{
	public:
		class Device : public GraphicsDevice
		{
		public:
			VkPhysicalDevice physicalDevice_;
			VkDevice logicalDevice_;
		};

		class Pipeline : public GraphicsPipeline
		{
		private:
			VkShaderModule vertexShaderModule_;
			VkShaderModule fragShaderModule_;
			std::vector<VkPipelineShaderStageCreateInfo> shaderStageInfos_;
			VkVertexInputBindingDescription vertexBindingDescription_{};
			std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions_;
			VkPipelineVertexInputStateCreateInfo vertexInputInfo_{};
			VkBuffer vertexBuffer_;
			VkDeviceMemory vertexBufferMemory_;
			VkBuffer indexBuffer_;
			VkDeviceMemory indexBufferMemory_;
			VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo_{};
			VkViewport viewport_{};
			VkRect2D scissor_{};
			VkPipelineViewportStateCreateInfo viewportInfo_{};
			VkPipelineRasterizationStateCreateInfo rasterizerInfo_{};
			VkPipelineMultisampleStateCreateInfo multisamplingInfo_{};
			VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
			VkPipelineColorBlendAttachmentState colorBlendAttachment_{};
			VkPipelineColorBlendStateCreateInfo colorBlendingInfo_{};

			VkPipelineLayout pipelineLayout_;
			VkPipeline instance_;

		public:
			Pipeline(const Device& _device, const PipelineState& _pipelineState, VkRenderPass _renderPass);

		public:
			void Release(const Device& _device);

			VkPipeline GetInstance() const;
			VkBuffer GetVertexBuffer() const;

		private:
			void CreatePipelineLayout(const Device& _device);
			void CreateShaderStageInfos(const Device& _device, const PipelineState& _pipelineState);
			void CreateVertexInputCreateInfo(const PipelineState& _pipelineState);
			void CreateVertexBuffer(const Device& _device, const PipelineState& _pipelineState);
			void CreateInputAssemblyCreateInfo(const PipelineState& _pipelineState);
			void CreateViewportCreateInfo(const PipelineState& _pipelineState);
			void CreateRasterizationCreateInfo(const PipelineState& _pipelineState);
			void CreateMultisampleCreateInfo(const PipelineState& _pipelineState);
			void CreateDepthStencilCreateInfo(const PipelineState& _pipelineState);
			void CreateColorBlendCreateInfo(const PipelineState& _pipelineState);
		};

	private:
#if _DEBUG
		inline static const std::vector<const char*> validationLayers =
		{
			"VK_LAYER_KHRONOS_validation"
		};
#endif

		inline static const std::vector<const char*> requiredExtensionNames =
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		};

		static constexpr size_t frameConcurrency = 2;

	private:
		GLFWwindow& window_;

		VkInstance instance_;
		VkSurfaceKHR surface_;

		std::optional<uint32_t> graphicsFamilyIndex_;
		std::optional<uint32_t> presentFamilyIndex_;

		VkQueue graphicsQueue_;
		VkQueue presentationQueue_;

		VkSwapchainKHR swapChain_;
		std::vector<VkImage> swapChainImages_;
		std::vector<VkImageView> swapChainImageViews_;
		std::vector<VkFramebuffer> swapChainFramebuffers_;
		
		VkRenderPass renderPass_;
		VkCommandPool commandPool_;

		std::array<VkCommandBuffer, frameConcurrency> commandBuffers_;
		std::array<VkSemaphore, frameConcurrency> imageAvailableSemaphores_;
		std::array<VkSemaphore, frameConcurrency> renderFinishedSemaphores_;
		std::array<VkFence, frameConcurrency> inFlightFences_;

		mutable size_t currentFrame_ = 0;

		uint32_t width_;
		uint32_t height_;

	public:
		Vulkan(GLFWwindow& _window);
		~Vulkan();

	public:
		virtual void BindPipeline(const PipelineState& _pipelineState) override;
		virtual void Execute() const override;

	private:
		void RetrieveFrameBufferSize();
		void CreateVulkanInstance();
		void CreateSurface();
		void SelectPhysicalDevice();
		void CreateLogicalDevice();
		void CreateSwapChain();
		void CreateRenderPass();
		void CreateFrameBuffer();
		void CreateCommandPool();
		void CreateCommandBuffer();
		void CreateSyncObjects();

		void InitializeSwapChainImages();
		void FindQueueFamilyIndex();
		void RecordCommandBuffer(const size_t _bufferIndex) const;

		static bool IsDeviceSuitable(VkPhysicalDevice _device);
	};
}