#pragma once
#include "common.h"

namespace io::graphics
{
	class RenderTarget
	{
	public:
		struct Layout
		{
			uint32_t width_ = 0;
			uint32_t height_ = 0;
			std::vector<ShaderDescriptor::Output> attachments_;
		};

	public:
		virtual ~RenderTarget() {}
	};
}