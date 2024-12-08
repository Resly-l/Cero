#pragma once
#include "common.h"
#include "utility/byte_buffer.h"
#include "utility/forward_declaration.h"
#include "utility/log.h"
#include "shader.h"
#include <string_view>
#include <array>

namespace graphics
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
			ShaderDescriptor shaderDescriptor_;
		};

	protected:
		std::array<std::shared_ptr<ShaderBinding>, 512> shaderBindings_;
		ShaderDescriptor shaderDescriptor_;

	public:
		Pipeline(const Layout& _layout) : shaderDescriptor_(_layout.shaderDescriptor_) {}
		virtual ~Pipeline() {};

	public:
		virtual std::shared_ptr<RenderTarget> CreateRenderTarget(uint32_t _width, uint32_t _height) const = 0;
		virtual bool BindShaderBinding(std::shared_ptr<ShaderBinding> _shaderBinding, uint32_t _slot)
		{
			using utility::Log;

			if (_slot >= shaderDescriptor_.bindings_.size())
			{
				std::cout << Log::Format(Log::Category::graphics, Log::Level::warning, "Tried to bind with invalid slot to pipeline") << std::endl;
				return false;
			}

			if (shaderDescriptor_.bindings_[_slot].type_ != _shaderBinding->type_)
			{
				std::cout << Log::Format(Log::Category::graphics, Log::Level::warning, "Tried to bind different binding type to pipeline") << std::endl;
				return false;
			}

			shaderBindings_[_slot] = _shaderBinding;
			return true;
		}
	};
}