#pragma once
#include <string>
#include "utility/byte_buffer.h"

namespace file
{
	/*
	* the reason why model is separated in graphics and file is,
	* since actual creation happens inside graphics api's CreateModel() function,
	* we needed some place to implement loading part of the model to prevent api implementing same thing again
	* = to separate loading and rendering part
	* 
	* by doing so, we can change loading part and graphics part separatly
	* ex) if we wanted to change loading library
	*/
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