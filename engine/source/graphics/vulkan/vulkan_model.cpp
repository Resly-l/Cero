#include "vulkan_model.h"
#include "vulkan_mesh.h"
#include "vulkan_material.h"

namespace graphics
{
    VulkanModel::VulkanModel(Initializer _initializer, const Model::Layout& _modelLayout)
    {
		for (const file::Material& materialFile : _modelLayout.modelFile_.materials_)
		{
			VulkanMaterial::Initializer initializer{};
			initializer.logicalDevice_ = _initializer.logicalDevice_;

			Material::Layout materialLayout;
			materialLayout.materiallFile_ = materialFile;

			materials_.push_back(std::make_shared<VulkanMaterial>(std::move(initializer), materialLayout));
		}

		for (const file::Mesh& meshFile : _modelLayout.modelFile_.meshes_)
		{
			Mesh::Layout meshLayout;
			meshLayout.vertices_ = meshFile.vertices_;
			meshLayout.indices_ = meshFile.indices_;
			
			auto mesh = std::make_shared<VulkanMesh>(_initializer.logicalDevice_, _initializer.physicalDevice_, _initializer.tranferQueue_, _initializer.transferCommandPool_, meshLayout);

			if (meshFile.materialIndex_ < materials_.size())
			{
				mesh->SetMaterial(materials_[meshFile.materialIndex_]);
			}

			meshes_.push_back(mesh);
		}
    }
}