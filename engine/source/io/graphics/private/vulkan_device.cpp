#include "vulkan_device.h"
#include "utility/stl.h"

namespace io::graphics
{
	void VulkanDevice::Initialize(VkInstance _instance, VkSurfaceKHR _surface)
	{
		SelectPhysicalDevice(_instance);
		FindQueueFamilyIndices(_surface);
		CreateLogicalDevice();
	}

	void VulkanDevice::Release()
	{
		vkDestroyDevice(logicalDevice_, nullptr);
	}

	VkPhysicalDevice VulkanDevice::GetPhysicalDevice() const
	{
		return physicalDevice_;
	}

	VkDevice VulkanDevice::GetLogicalDevice() const
	{
		return logicalDevice_;
	}

	uint32_t VulkanDevice::GetGraphicsQueueFamilyIndex() const
	{
		return graphicsQueueFamilyIndex_;
	}

	uint32_t VulkanDevice::GetPresentQueueFamilyIndex() const
	{
		return presentQueueFamilyIndex_;
	}

	void VulkanDevice::SelectPhysicalDevice(VkInstance _instance)
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

		for (const auto& device : devices)
		{
			if (IsDeviceSuitable(device))
			{
				physicalDevice_ = device;
				return;
			}
		}

		if (physicalDevice_ == nullptr)
		{
			throw std::runtime_error("failed to find GPU with Vulkan support");
		}
	}

	void VulkanDevice::FindQueueFamilyIndices(VkSurfaceKHR _surface)
	{
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueFamilyCount, queueFamilies.data());

		bool foundGraphicsQueue = false;
		bool foundPresentQueue = false;
		for (uint32_t i = 0; i < queueFamilyCount; i++)
		{
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice_, i, _surface, &presentSupport);

			if (presentSupport && !foundGraphicsQueue)
			{
				presentQueueFamilyIndex_ = i;
				foundGraphicsQueue = true;
			}

			if ((queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && !foundPresentQueue)
			{
				graphicsQueueFamilyIndex_ = i;
				foundPresentQueue = true;
			}
		}

		if (!foundGraphicsQueue || !foundPresentQueue)
		{
			throw std::runtime_error("failed to find queue families");
		}
	}

	void VulkanDevice::CreateLogicalDevice()
	{
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> queueFamilyIndices = { graphicsQueueFamilyIndex_, presentQueueFamilyIndex_ };

		for (uint32_t index : queueFamilyIndices)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = index;
			queueCreateInfo.queueCount = 1;
			float queuePriority = 1.0f;
			queueCreateInfo.pQueuePriorities = &queuePriority;

			queueCreateInfos.push_back(queueCreateInfo);
		}

		const std::vector<const char*> requiredExtensionNames = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		VkPhysicalDeviceFeatures deviceFeatures{};
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = (uint32_t)requiredExtensionNames.size();
		createInfo.ppEnabledExtensionNames = requiredExtensionNames.data();
		createInfo.enabledLayerCount = 0;

		if (vkCreateDevice(physicalDevice_, &createInfo, nullptr, &logicalDevice_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create logical device!");
		}
	}

	bool VulkanDevice::IsDeviceSuitable(VkPhysicalDevice _device)
	{
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(_device, &deviceProperties);

		if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			return false;
		}

		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(_device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(_device, nullptr, &extensionCount, availableExtensions.data());

		std::vector<const char*> requiredExtensionNames = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, };

		for (const std::string extensionName : requiredExtensionNames)
		{
			auto it = std::find_if(availableExtensions.begin(), availableExtensions.end(),
				[extensionName](const VkExtensionProperties& extension)
				{
					return extensionName == extension.extensionName;
				});

			if (it == availableExtensions.end())
			{
				return false;
			};
		}

		return true;
	}
}