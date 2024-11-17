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
			bool persistentMapping_ = false;
		};

	public:
		virtual void Update(const void* _data) = 0;
	};
}