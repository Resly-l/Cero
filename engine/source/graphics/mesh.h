#pragma once
#include "utility/byte_buffer.h"
#include "utility/forward_declaration.h"
#include <memory>

namespace graphics
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

	protected:
		uint32_t numVertices_;
		uint32_t numIndices_;
		std::shared_ptr<Material> material_;

	public:
		uint32_t GetNumVertices() const { return numVertices_; };
		uint32_t GetNumIndices() const { return numIndices_; };
		void SetMaterial(std::shared_ptr<Material> _material) { material_ = _material; }
	};
}