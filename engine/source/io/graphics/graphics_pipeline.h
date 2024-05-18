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

	class RenderTarget
	{
	public:
		struct AttachmentDescription
		{
			ImageFormat format_ = ImageFormat::NONE;
			ImageUsage usage_ = ImageUsage::NONE;
			uint32_t width_ = 0;
			uint32_t height_ = 0;
		};

		struct Layout
		{
			uint32_t width_ = 0;
			uint32_t height_ = 0;
			std::vector<AttachmentDescription> attachments_;
		};
	};

	class Mesh
	{
	public:
		struct Layout
		{
			utility::ByteBuffer vertices_;
			std::vector<uint32_t> indices_;
		};
	};

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
	};
}