#include "model.h"
#include "utility/log.h"
#include "../thirdparty/assimp/Importer.hpp"
#include "../thirdparty/assimp/scene.h"
#include "../thirdparty/assimp/postprocess.h"
#include "math/vector.h"

#if _DEBUG
#pragma comment(lib, "assimp/bin/assimp-vc143-mtd.lib")
#pragma comment(lib, "zlib/zlibstaticd.lib")
#else
#pragma comment(lib, "assimp/bin/assimp-vc143-mt.lib")
#pragma comment(lib, "zlib/zlibstatic.lib")
#endif

using utility::Log;

namespace file
{
	bool Model::Load(const std::string& _path)
	{
		size_t separater = _path.find_last_of('/');
		std::string parentDir(_path.begin(), _path.begin() + ((std::string::npos == separater) ? 0 : _path.find_last_of('/') + 1));

		static Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(_path.data(),
			aiProcess_ConvertToLeftHanded |
			aiProcess_JoinIdenticalVertices |
			aiProcess_Triangulate |
			aiProcess_GenNormals |
			aiProcess_GenUVCoords |
			aiProcess_CalcTangentSpace |
			aiProcess_LimitBoneWeights |
			aiProcess_ValidateDataStructure |
			aiProcess_OptimizeMeshes |
			aiProcess_SplitLargeMeshes |
			aiProcess_ImproveCacheLocality
		);
		importer.ApplyPostProcessing(aiProcess_CalcTangentSpace);

		if (!scene)
		{
			std::cout << Log::Format(Log::Category::file, Log::Level::warning, "failed to load model");
			return false;
		}

		utility::ByteBuffer::Layout vertexLayout;
		vertexLayout.AddAttribute<math::Float3>(); // position
		vertexLayout.AddAttribute<math::Float3>(); // normal
		vertexLayout.AddAttribute<math::Float3>(); // tangent
		vertexLayout.AddAttribute<math::Float3>(); // bitangent
		vertexLayout.AddAttribute<math::Float2>(); // texcoord

		for (uint32_t i = 0; i < scene->mNumMeshes; i++)
		{
			Mesh mesh;
			mesh.vertices_.SetLayout(vertexLayout);

			for (uint32_t j = 0; j < (*scene->mMeshes[i]).mNumVertices; j++)
			{
				auto vertex = mesh.vertices_.Add();
				vertex.Get<math::Float3>(0) = *(math::Float3*)(&(*scene->mMeshes[i]).mVertices[j]);
				vertex.Get<math::Float3>(1) = *(math::Float3*)(&(*scene->mMeshes[i]).mNormals[j]);

				if (scene->mMeshes[i]->HasTangentsAndBitangents())
				{
					vertex.Get<math::Float3>(2) = *(math::Float3*)(&(*scene->mMeshes[i]).mTangents[j]);
					vertex.Get<math::Float3>(3) = *(math::Float3*)(&(*scene->mMeshes[i]).mBitangents[j]);
				}
				else
				{
					vertex.Get<math::Float3>(2) = math::Float3{};
					vertex.Get<math::Float3>(3) = math::Float3{};
				}

				if (scene->mMeshes[i]->HasTextureCoords(0))
				{
					vertex.Get<math::Float2>(4) = *(math::Float3*)(&(*scene->mMeshes[i]).mTextureCoords[0][j]);
				}
				else
				{
					vertex.Get<math::Float2>(4) = math::Float2{};
				}
			}

			mesh.indices_.reserve((size_t)(*scene->mMeshes[i]).mNumFaces * 3);
			for (uint32_t uFace = 0; uFace < (*scene->mMeshes[i]).mNumFaces; uFace++)
			{
				mesh.indices_.push_back((*scene->mMeshes[i]).mFaces[uFace].mIndices[0]);
				mesh.indices_.push_back((*scene->mMeshes[i]).mFaces[uFace].mIndices[1]);
				mesh.indices_.push_back((*scene->mMeshes[i]).mFaces[uFace].mIndices[2]);
			}

			mesh.materialIndex_ = (*scene->mMeshes[i]).mMaterialIndex;
			meshes_.push_back(std::move(mesh));
		}

		for (uint32_t i = 0; i < scene->mNumMaterials; i++)
		{
			Material material;

			aiString path;
			if (scene->mMaterials[i]->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &path) == aiReturn_SUCCESS)
			{
				material.diffuseMapPath_ = parentDir + path.C_Str();
			}
			if (scene->mMaterials[i]->GetTexture(aiTextureType::aiTextureType_NORMALS, 0, &path) == aiReturn_SUCCESS)
			{
				material.normalMapPath_ = parentDir + path.C_Str();
			}

			materials_.push_back(std::move(material));
		}

		return true;
	}

	bool Model::IsLoaded() const
	{
		return !meshes_.empty();
	}
}