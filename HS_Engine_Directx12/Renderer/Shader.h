#ifndef SHADER_H
#define SHADER_H
#include <initializer_list>
#include <string>
#include <utility>
#include "../Engine/stdafx.h"
#include <wrl/client.h>


using Microsoft::WRL::ComPtr;

enum class E_ShaderType
{
	VERTEX_SHADER,
	PIXEL_SHADER,
	HULL_SHADER,
	MESH_SHADER,
	DOMAIN_SHADER,
	GEOMETRY_SHADER,
	COMPUTE_SHADER,
	NONE,
};

namespace HS_Engine
{

	class Shader
	{
	public:
		Shader(std::initializer_list<std::pair<E_ShaderType, std::wstring>> shaders, bool is_mesh_shader);
		void MeshShaderInit();
		void DefaultShaderInit();
		void ComplieShader(std::pair<E_ShaderType, std::wstring> path);
		ComPtr<ID3D12PipelineState> GetPipelineState() const { return pipelineStateObject; }

	private:
		ComPtr<ID3D12RootSignature> rootSignature;
		ComPtr<ID3D12PipelineState> pipelineStateObject;
		std::vector<std::pair<E_ShaderType,ComPtr<ID3DBlob>>> mShaderProgram;
		ComPtr<ID3D12Resource> m_renderTargets[3];
		ComPtr<ID3D12Resource> m_depthStencil;
	};
}

#endif
