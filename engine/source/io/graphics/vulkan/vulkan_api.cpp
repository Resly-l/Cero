#include "vulkan_api.h"
#include "vulkan_bootstrap.h"

namespace io::graphics
{
	VulkanAPI::VulkanAPI(GLFWwindow& _window)
		: window_(_window)
	{
		instance_ = VulkanBootstrap::CreateVulkanInstance();
		surface_ =  VulkanBootstrap::CreateSurface(instance_, &_window);

		device_.Initialize(instance_, surface_);

		int width = 0;
		int height = 0;
		glfwGetFramebufferSize(&window_, &width, &height);

		swapChain_.Initialize(device_, surface_, (uint32_t)width, (uint32_t)height);
	}

	VulkanAPI::~VulkanAPI()
	{
		swapChain_.Release(device_);

		deviceResources_.clear();

		device_.Release();

		vkDestroySurfaceKHR(instance_, surface_, nullptr);
		vkDestroyInstance(instance_, nullptr);
	}

	std::shared_ptr<Pipeline> VulkanAPI::CreatePipeline(const PipelineState& _pipelineState)
	{
		deviceResources_.push_back(std::make_shared<VulkanPipeline>(device_, _pipelineState, swapChain_));
		return std::static_pointer_cast<VulkanPipeline>(deviceResources_.back());
	}

	std::shared_ptr<RenderTarget> VulkanAPI::CreateRenderTarget(const PipelineState& _pipelineState)
	{
		return std::shared_ptr<RenderTarget>();
	}

	void VulkanAPI::BindPipeline(std::shared_ptr<Pipeline> _pipeline)
	{
		pipeline_ = std::static_pointer_cast<VulkanPipeline>(_pipeline).get();
	}

	void VulkanAPI::BindRenderTarget(std::shared_ptr<RenderTarget> _renderTarget)
	{

	}

	void VulkanAPI::BeginFrame()
	{
		device_.BeginCommandBuffer(swapChain_);
		device_.BeginRenderPass(swapChain_);
	}

	void VulkanAPI::Draw()
	{
		if (pipeline_ == nullptr)
		{
			return;
		}

		device_.Execute(device_, *pipeline_);

		ReleaseUnusedPipelines();
	}

	void VulkanAPI::EndFrame()
	{
		device_.Present(device_, swapChain_);
	}

	void VulkanAPI::ReleaseUnusedPipelines()
	{
		std::erase_if(deviceResources_,
			[](const std::shared_ptr<DeviceResource>& _deviceResources)
			{
				return _deviceResources.use_count() == 1;
			});
	}
}