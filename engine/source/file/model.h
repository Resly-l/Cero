#pragma once
#include <string>
#include "utility/byte_buffer.h"

namespace file
{
	struct Model
	{
		utility::ByteBuffer vertices_;
		std::vector<uint32_t> indices_;

		std::string diffuseMapPath_;

		bool Load(const std::string& _path);
		bool IsLoaded() const;
	};
}