#pragma once
#include "utility/byte_buffer.h"
#include "shader.h"

namespace graphics
{
	class UniformBuffer : public ShaderBinding
	{
	public:
		struct Layout
		{
			uint32_t size_ = 0;
			uint32_t numElements_ = 1;
		};

	public:
		virtual void Update(const void* _data) const = 0;
	};
}