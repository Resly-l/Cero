#include "model.h"
#include "utility/log.h"
#include "thirdparty/assimp/Importer.hpp"
#include "thirdparty/assimp/scene.h"
#include "thirdparty/assimp/postprocess.h"
#include "core/math/vector.h"

#if _DEBUG
#pragma comment(lib, "assimp/bin/assimp-vc143-mtd.lib")
#pragma comment(lib, "zlib/zlibstaticd.lib")
#else
#pragma comment(lib, "assimp/bin/assimp-vc143-mt.lib")
#pragma comment(lib, "zlib/zlibstatic.lib")
#endif

using utility::Log;

namespace io::file
{
	bool Model::Load(std::string_view _path)
	{
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

		if (!scene)
		{
			std::cout << Log::Format(Log::Category::file, Log::Level::warning, "failed to load model");
			return false;
		}

		utility::ByteBuffer::Layout vertexLayout;
		vertexLayout.AddAttribute<math::Float3>();
		vertexLayout.AddAttribute<math::Float3>();
		vertexLayout.AddAttribute<math::Float3>();
		vertexLayout.AddAttribute<math::Float3>();
		vertexLayout.AddAttribute<math::Float2>();
		vertices_.SetLayout(vertexLayout);

		for (uint32_t i = 0; i < scene->mNumMeshes; i++)
		{
			auto& mesh = *scene->mMeshes[i];
			for (uint32_t j = 0; j < mesh.mNumVertices; j++)
			{
				auto vertex = vertices_.Add();
				vertex.Get<math::Float3>(0) = *(math::Float3*)(&mesh.mVertices[j]);
				vertex.Get<math::Float3>(1) = *(math::Float3*)(&mesh.mNormals[j]);
				vertex.Get<math::Float3>(2) = *(math::Float3*)(&mesh.mTangents[j]);
				vertex.Get<math::Float3>(3) = *(math::Float3*)(&mesh.mBitangents[j]);
				vertex.Get<math::Float2>(4) = *(math::Float3*)(&mesh.mTextureCoords[0][j]);
			}

			indices_.reserve((size_t)mesh.mNumFaces * 3);
			for (uint32_t uFace = 0; uFace < mesh.mNumFaces; uFace++)
			{
				indices_.push_back(mesh.mFaces[uFace].mIndices[0]);
				indices_.push_back(mesh.mFaces[uFace].mIndices[1]);
				indices_.push_back(mesh.mFaces[uFace].mIndices[2]);
			}
		}

		for (uint32_t i = 0; i < scene->mNumMaterials; i++)
		{

		}



		return true;
	}

	bool Model::IsLoaded() const
	{
		return vertices_.GetSizeInBytes() != 0;
	}
}