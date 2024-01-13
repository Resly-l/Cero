#pragma once
#include "utility/stl.h"
#include "utility/byte_buffer.h"

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

	struct PipelineState
	{
		PrimitiveTopology primitiveTopology = PrimitiveTopology::NONE;
		ComparisonFunc depthFunc = ComparisonFunc::NONE;
		ComparisonFunc stencilFunc = ComparisonFunc::NONE;
		bool depthStencilReadOnly = false;
		RasterizerState rasterizerState = RasterizerState::NONE;
		BlendState blendState = BlendState::NONE;
		Viewport viewport_;

		std::wstring_view vertexShaderPath;
		std::wstring_view pixelShaderPath;

		utility::ByteBuffer vertexBuffer;
		std::vector<uint32_t> indexBuffer;
	};
}