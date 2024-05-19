#pragma once
#include "utility/byte_buffer.h"

namespace io::graphics
{
	class Mesh
	{
	public:
		struct Layout
		{
			utility::ByteBuffer vertices_;
			std::vector<uint32_t> indices_;
		};

	public:
		virtual ~Mesh() {};

	public:
		virtual uint32_t GetNumVertices() const = 0;
		virtual uint32_t GetNumIndices() const = 0;
	};
}