#pragma once
#include "common.h"
#include "utility/byte_buffer.h"

namespace io::graphics
{
	class Pipeline
	{
	public:
		struct Layout
		{
			PrimitiveTopology primitiveTopology_ = PrimitiveTopology::NONE;
			ComparisonFunc depthFunc_ = ComparisonFunc::NONE;
			ComparisonFunc stencilFunc_ = ComparisonFunc::NONE;
			bool depthStencilReadOnly_ = false;
			RasterizerState rasterizerState_ = RasterizerState::NONE;
			BlendState blendState_ = BlendState::NONE;
			Viewport viewport_;

			std::wstring_view vertexShaderPath_;
			std::wstring_view pixelShaderPath_;
			utility::ByteBuffer::Layout vertexInputLayout_;
		};

	public:
		virtual ~Pipeline() {};
	};
}