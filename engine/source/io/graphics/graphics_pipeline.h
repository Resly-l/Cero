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
	};

	enum class ImageUsage
	{
		NONE,
		COLOR_ATTACHMENT,
		DEPTH_STENCIL
	};

	class Framebuffer
	{
	public:
		struct AttachmentDescription
		{
			ImageFormat format_ = ImageFormat::NONE;
			ImageUsage usage_ = ImageUsage::NONE;
			uint32_t width_ = 0;
			uint32_t height_ = 0;
			// using exist attachment (ex, when using swapchain images, don't need to create new image)
			void* attachmentHandle_ = nullptr;
		};

		struct Layout
		{
			uint32_t width_ = 0;
			uint32_t height_ = 0;
			std::vector<AttachmentDescription> attachments_;
		};
	};

	class Pipeline
	{
	public:
		struct Layout
		{
			PrimitiveTopology primitiveTopology = PrimitiveTopology::NONE;
			ComparisonFunc depthFunc = ComparisonFunc::NONE;
			ComparisonFunc stencilFunc = ComparisonFunc::NONE;
			bool depthStencilReadOnly = false;
			RasterizerState rasterizerState = RasterizerState::NONE;
			BlendState blendState = BlendState::NONE;
			Viewport viewport_;
			std::vector<Framebuffer::Layout> frameBufferLayouts;

			std::wstring_view vertexShaderPath;
			std::wstring_view pixelShaderPath;
			utility::ByteBuffer::Layout vertexInputLayout;
		};
	};
}