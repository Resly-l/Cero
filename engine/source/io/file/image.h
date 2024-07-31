#pragma once
#include "utility/stl.h"

namespace io::file
{
	struct Image
	{
		std::vector<uint8_t> colors_;
		uint32_t width_ = 0;
		uint32_t height_ = 0;

		bool Load(std::string_view _path);
		bool IsLoaded() const;
		uint8_t At(uint32_t _x, uint32_t _y) const;
	};
}