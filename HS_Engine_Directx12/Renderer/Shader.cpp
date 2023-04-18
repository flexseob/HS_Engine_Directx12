#include "Shader.h"

#include "../Engine/Context.h"
#include "../Engine/Engine.h"
namespace HS_Engine
{
	Shader::Shader(std::initializer_list<std::pair<E_ShaderType, std::wstring>> shaders)
	{
		auto& context = Engine::Instance().GetWindow()->GetWindowData().m_context;
		auto* device = context->device;

		//TODO : dynamic root signature or initalize the root signature parameter
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		HRESULT hr;

		hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
		if(FAILED(hr))
		{
			//TODO error handling
		}

		hr = device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
		if(FAILED(hr))
		{
			//TODO error Handling
		}

			for (auto& shader : shaders)
			{



			}
	}

	void Shader::CompleShader(std::pair<E_ShaderType,std::wstring> path)
	{
#if defined(_DEBUG)
		// Enable better shader debugging with the graphics debugging tools.
		UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		UINT compileFlags = 0;
#endif
		HRESULT hr;
		ComPtr<ID3DBlob> shader_code;
		
		switch(path.first)
		{
		case E_ShaderType::VERTEX_SHADER:
			hr = D3DCompileFromFile(path.second.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &shader_code, nullptr);
			break;
		case E_ShaderType::COMPUTE_SHADER:
			
			break;
		case E_ShaderType::DOMAIN_SHADER:
			
			break;
		case E_ShaderType::GEOMETRY_SHADER:
			
			break;
		case E_ShaderType::HULL_SHADER:
			
			break;
		case E_ShaderType::PIXEL_SHADER:
			hr = D3DCompileFromFile(path.second.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &shader_code, nullptr);
			break;
		case E_ShaderType::TESSELLATOR_SHADER:
			
			break;
		default:;
		}
		if(FAILED(hr))
		{
			//TODO : Error Handling
		}




	}

}

