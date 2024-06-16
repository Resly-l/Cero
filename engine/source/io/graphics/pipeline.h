#pragma once
#include "common.h"
#include "utility/byte_buffer.h"
#include "utility/forward_declaration.h"

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
			ShaderDescriptor descriptor_;
		};

	public:
		virtual ~Pipeline() {};

	public:
		virtual std::shared_ptr<RenderTarget> CreateRenderTarget(uint32_t _width, uint32_t _height) const = 0;
		virtual std::shared_ptr<RenderTarget> CreateRenderTarget(uint32_t _width, uint32_t _height, std::shared_ptr<ImageView> _imageView) const = 0;

		virtual void UpdateUniformBuffer(uint32_t _index, const utility::ByteBuffer& _buffer) = 0;
	};
}