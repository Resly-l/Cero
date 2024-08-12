#pragma once
#include "graphics/graphics_api.h"
#include <vulkan/vulkan.h>
#include <unordered_map>

namespace graphics
{
	struct VulkanAttachment
	{
		VkImageView imageView_;
		std::optional<VkImage> image_;
		std::optional<VkDeviceMemory> memory_;
	};

	class VulkanRenderTarget : public RenderTarget
	{
	private:
		VkDevice logicalDevice_;
		uint32_t width_;
		uint32_t height_;
		std::vector <VulkanAttachment> attachments_;
		std::unordered_map<VkRenderPass, VkFramebuffer> framebuffers_;

	public:
		VulkanRenderTarget(VkDevice _logicalDevice, const RenderTarget::Layout& _renderTargetLayout);
		VulkanRenderTarget(VkDevice _logicalDevice, uint32_t _width, uint32_t _height, VkImageView _outputImageView);
		~VulkanRenderTarget();

	public:
		void AddAttachment(ShaderDescriptor::Output _description);
		void Bind(VkRenderPass _renderPass);

		uint32_t GetNumAttachments() const;
		std::vector<VkImageView> GetImageViews() const;
		VkFramebuffer GetFramebuffer(VkRenderPass _renderPass) const;

	private:
		static VkImageAspectFlags GetAspectMask(VkFormat _format, VkImageUsageFlags _usage);
	};
}