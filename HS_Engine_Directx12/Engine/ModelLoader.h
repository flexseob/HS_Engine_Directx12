#ifndef MODELLOADER_H
#define MODELLOADER_H
#include <filesystem>
#include <set>
#include <assimp/Importer.hpp>
#include <DirectXMesh/DirectXMesh.h>
#include <DirectXTex/DirectXTex.h>

namespace HS_Engine
{
	namespace Asset
	{
		class AssetImporter
		{
		public:
			bool SupportsExtension(const std::filesystem::path& Path)
			{
				std::wstring Extension = Path.extension().wstring();
				return std::ranges::find_if(
					mSupportedExtension,
					[&](const std::wstring& SupportedExtension)
					{
						return SupportedExtension == Extension;
					}) != mSupportedExtension.end();
			}

		protected:
			std::set<std::wstring> mSupportedExtension;
		};


		class MeshImporter : public AssetImporter
		{
		public:
			MeshImporter();


			struct MeshData
			{
				size_t mNumVertices;
				size_t mNumIndices;
				size_t mNumMeshlets;
				size_t mNumUniqueVertexIndices;
				size_t mNumPrimitiveIndices;
			};
			









		};





	}
	



}

#endif
