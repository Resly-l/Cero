#pragma once
#include "shader.h"
#include "file/image.h"
#include <optional>
#include <string_view>

namespace graphics
{
	class Texture : public ShaderBinding
	{
	public:
		enum class InitializationType
		{
			FILE,
			BUFFER,
		};

		struct Layout
		{
			ImageFormat format_ = ImageFormat::R8G8B8A8_UNORM;
			uint32_t size_ = 0;
			uint32_t numElements_ = 1;
			InitializationType initializationType_ = InitializationType::FILE;
			std::string_view imagePath_;
			std::optional<file::Image> buffer_;
		};

	public:
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
	};
}