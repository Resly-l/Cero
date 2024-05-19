#pragma once
#include "common.h"

namespace io::graphics
{
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

	public:
		virtual ~RenderTarget() {}
	};
}