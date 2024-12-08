#pragma once
#include <string>
#include "utility/byte_buffer.h"

namespace file
{
	struct Material
	{
		std::string diffuseMapPath_;
		std::string normalMapPath_;
	};

	struct Mesh
	{
		utility::ByteBuffer vertices_;
		std::vector<uint32_t> indices_;
		uint32_t materialIndex_ = 0;
	};

	struct Model
	{
	public:
		std::vector<Material> materials_;
		std::vector<Mesh> meshes_;

		bool Load(const std::string& _path);
		bool IsLoaded() const;
	};
}