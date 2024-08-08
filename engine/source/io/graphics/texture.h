#pragma once
#include "common.h"
#include "shader.h"
#include <string_view>

namespace io::graphics
{
	class Texture : public ShaderResource
	{
	public:
		struct Layout
		{
			ShaderBinding::Stage stage_{};
			ImageFormat format_ = ImageFormat::R8G8B8A8_UNORM;
			uint32_t size_ = 0;
			uint32_t slot_ = 0;
			uint32_t numElements_ = 1;
			std::string_view imagePath_;
		};

	public:
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
	};
}