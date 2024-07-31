#pragma once
#include "common.h"

namespace io::graphics
{
	class Texture
	{
	public:
		struct Layout
		{
			std::string_view imagePath_;
		};

	public:
		virtual ~Texture() {};

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
	};
}