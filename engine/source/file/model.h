#pragma once
#include <string>
#include "utility/byte_buffer.h"

namespace file
{
	struct Model
	{
		utility::ByteBuffer vertices_;
		std::vector<uint32_t> indices_;

		bool Load(std::string_view _path);
		bool IsLoaded() const;
	};
}