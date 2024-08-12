#pragma once
#include <source_location>
#include <format>
#include "../thirdparty/vk_bootstrap/VkBootstrap.h"

namespace graphics
{
	struct VulkanResult
	{
		VkResult result_;
		std::string message_;
		std::source_location location_;

		VulkanResult(VkResult _result, std::source_location _location = std::source_location::current())
			: result_(_result)
			, location_(_location)
		{}

		template <typename T>
		VulkanResult(T _result, std::source_location _location = std::source_location::current())
			: result_(_result ? VK_SUCCESS : _result.vk_result())
			, message_(_result ? std::string{} : _result.error().message())
			, location_(_location)
		{}
	};

	struct VulkanResultChecker
	{
		inline static VulkanResultChecker& Get()
		{
			static VulkanResultChecker checker;
			return checker;
		};

		void Check(const VulkanResult& _result)
		{
			if (_result.result_ != VK_SUCCESS)
			{
				throw std::runtime_error(std::format("VkResult failed at \n file:{}\n\nfunction:{}\n\nline:{}\n\nmessage:{}",
					_result.location_.file_name(),
					_result.location_.function_name(),
					_result.location_.line(),
					_result.message_));
			}
		}
	};

	inline void operator >>(VulkanResult _result, VulkanResultChecker _checker)
	{
		_checker.Check(_result);
	}

	template <typename Builder, typename Func, typename... Params>
	auto Build(Builder& _builder, Func _func, Params... _params)
	{
		vkb::Result result = (_builder.*_func)(std::forward<Params>(_params)...);
		result >> VulkanResultChecker::Get();
		return result.value();
	}
}