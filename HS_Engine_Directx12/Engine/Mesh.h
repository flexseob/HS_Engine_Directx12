#ifndef MESH_H
#define MESH_H
#include <filesystem>
#include "stdafx.h"
#include "Core.h"
#include <DirectXMesh/DirectXMesh.h>
using namespace DirectX;
using namespace Microsoft::WRL;

namespace HS_Engine
{
	namespace Asset
	{
		struct MeshOption
		{
			MeshOption()
			{
				XMStoreFloat4x4(&matrix, XMMatrixIdentity());
			}

			std::filesystem::path path;

			XMFLOAT3 translation = { 0.f,0.f,0.f };
			XMFLOAT3 rotation = { 0.f,0.f,0.f };
			XMFLOAT3 scale = { 1.f,1.f,1.f };
			XMFLOAT4X4 matrix;
		};


		class Mesh
		{
			void SetVertices(std::vector<Vertex>&& vertices);
			void SetIndices(std::vector<unsigned int>&& indices);
			void SetMeshLets(std::vector<Meshlet>&& meshlets);
			void SetUniqueVertexIndices(std::vector<unsigned char>&& uniqueVertexIndices);
			void SetPrimitiveIndices(std::vector<MeshletTriangle>&& primitiveIndices);


			MeshOption options;

			unsigned int numVertices = 0;
			unsigned int numIndices = 0;
			unsigned int numMeshlets = 0;
			unsigned int numVertexIndices = 0;
			unsigned int numPrimtives = 0;

			std::vector<Vertex> vertices;
			std::vector<unsigned int> indices;
			std::vector<Meshlet> meshlets;
			std::vector<unsigned char> uniquesVertexIndices;
			std::vector<MeshletTriangle> primitiveIndices;

			


		};



	}

}




#endif
