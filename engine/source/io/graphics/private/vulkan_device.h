#pragma once
#include <vulkan/vulkan.h>

namespace io::graphics
{
	class VulkanDevice
	{
	private:
		VkPhysicalDevice physicalDevice_;
		VkDevice logicalDevice_;

		uint32_t graphicsQueueFamilyIndex_;
		uint32_t presentQueueFamilyIndex_;

	public:
		void Initialize(VkInstance _instance, VkSurfaceKHR _surface);
		void Release();

		VkPhysicalDevice GetPhysicalDevice() const;
		VkDevice GetLogicalDevice() const;

		uint32_t GetGraphicsQueueFamilyIndex() const;
		uint32_t GetPresentQueueFamilyIndex() const;

	private:
		void SelectPhysicalDevice(VkInstance _instance);
		void FindQueueFamilyIndices(VkSurfaceKHR _surface);
		void CreateLogicalDevice();

		static bool IsDeviceSuitable(VkPhysicalDevice _device);
	};
}