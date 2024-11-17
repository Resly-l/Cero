#pragma once
#include "graphics/common.h"
#include <vector>
#include <memory>

namespace graphics
{
	enum class ShaderStage : uint8_t
	{
		VERTEX = 1 << 0,
		PIXEL = 1 << 1,
	};

	class ShaderBinding
	{
	public:
		enum class Type
		{
			UNIFORM_BUFFER,
			TEXTURE_2D,
		};

	public:
		Type type_;
		uint32_t numElements_ = 1;
	};

	struct ShaderDescriptor
	{
		struct Binding
		{
			ShaderBinding::Type type_;
			uint32_t numElements_ = 1;
			uint32_t slot_ = std::numeric_limits<uint32_t>::max();
			ShaderStage stage_;
		};

		struct Output
		{
			ImageFormat format_;
			ImageUsage usage_;
			ImageOperation loadOp_;
			ImageOperation storeOp_;
			uint32_t width_;
			uint32_t height_;
		};

		std::vector<Binding> bindings_;
		std::vector<Output> outputs;
	};

	enum class ReservedBindings : uint8_t
	{
		DIFFUSE_MAP,
		NORMAL_MAP,
	};

	// descending offset used in shader to prevent conflith with user defind offsets
	static constexpr uint32_t reservedBindingOffset = 50;
}