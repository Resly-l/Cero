#pragma once
#include "utility/byte_buffer.h"
#include "shader.h"

namespace io::graphics
{
	class UniformBuffer : public ShaderBinding
	{
	public:
		struct Layout
		{
			ShaderBinding::Stage stage_{};
			uint32_t size_ = 0;
			uint32_t slot_ = 0;
			uint32_t numElements_ = 1;
		};

	public:
		virtual ShaderBinding::Type GetType() const override { return Type::UNIFORM; }
		virtual void Update(const void* _data) const = 0;
	};
}