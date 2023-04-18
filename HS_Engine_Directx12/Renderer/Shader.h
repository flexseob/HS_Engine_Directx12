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
		Shader(std::initializer_list<std::pair<E_ShaderType, std::wstring>> shaders);

		void CompleShader(std::pair<E_ShaderType, std::wstring> path);
	private:
		ComPtr<ID3D12RootSignature> rootSignature;
		ComPtr<ID3D12PipelineState> pipelineStateObject;
		std::vector<std::pair<E_ShaderType,ComPtr<ID3DBlob>>> mShaderProgram;
	};
}

#endif
