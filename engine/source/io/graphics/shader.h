#pragma once
#include "io/graphics/common.h"
#include <vector>
#include <memory>

namespace io::graphics
{
	class ShaderBinding
	{
	public:
		enum class Type
		{
			UNIFORM,
			TEXTURE,
		};
		enum Stage : uint32_t
		{
			VERTEX = 1 << 0,
			PIXEL = 1 << 1,
		};

	public:
		virtual ~ShaderBinding() {}

		virtual Type GetType() const = 0;
	};

	struct ShaderDescriptor
	{
		struct Output
		{
			ImageFormat format_;
			ImageUsage usage_;
			ImageOperation loadOp_;
			ImageOperation storeOp_;
			uint32_t width_;
			uint32_t height_;
		};

		std::vector<std::shared_ptr<ShaderBinding>> bindings_;
		std::vector<Output> outputs;
	};
}