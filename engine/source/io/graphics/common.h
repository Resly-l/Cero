#pragma once
#include "utility/stl.h"

namespace io::graphics
{
	enum class PrimitiveTopology
	{
		NONE,
		LINE_LIST,
		TRIANGLE_LIST
	};

	enum class ComparisonFunc
	{
		NONE,
		LESS,
		LESS_EQUAL,
		EQUAL,
		NOT_EQUAL,
		GREATER,
		GREATER_EQUAL,
		ALWAYS
	};

	enum class RasterizerState
	{
		NONE,
		CULL_NONE,
		CULL_FRONT,
		CULL_BACK
	};

	enum class BlendState
	{
		NONE,
	};

	struct Viewport
	{
		uint32_t width_ = 0;
		uint32_t height_ = 0;
	};

	enum class ImageFormat
	{
		NONE,
		R8G8B8_UNORM,
		R8G8B8_NORM,
		R8G8B8A8_UNORM,
		R8G8B8A8_NORM,
		B8G8R8_UNORM,
		B8G8R8_NORM,
		B8G8R8A8_UNORM,
		B8G8R8A8_NORM,
		D32_SFLOAT,
		D32_SFLOAT_U8_UINT,
	};

	enum class ImageUsage
	{
		NONE,
		COLOR_ATTACHMENT,
		DEPTH_STENCIL
	};

	enum class ImageOperation
	{
		NONE,
		CLEAR,
		STORE,
	};

	struct ShaderDescriptor
	{
		struct Binding
		{
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

			Type type_;
			Stage stage_;
			uint32_t size_ = 0;
			uint32_t elementCount_ = 1; // 1 if not an array
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
}