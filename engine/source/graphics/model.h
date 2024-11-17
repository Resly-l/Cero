#pragma once
#include <memory>
#include "file/model.h"
#include "mesh.h"
#include "material.h"

namespace graphics
{
	class Model
	{
	public:
		struct Layout
		{
			// refer to file::Model's comment
			file::Model modelFile_;
		};

	public:
		virtual ~Model() {};

	protected:
		std::vector<std::shared_ptr<Mesh>> meshes_;
		std::vector<std::shared_ptr<Material>> materials_;
		
	public:
		const std::vector<std::shared_ptr<Mesh>>& GetMeshes() const { return meshes_; }
		const std::vector<std::shared_ptr<Material>>& GetMaterials() const { return materials_; }
	};
}