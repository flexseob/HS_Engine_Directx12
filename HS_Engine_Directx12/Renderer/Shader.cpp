#include "Shader.h"

#include "../Engine/Context.h"
#include "../Engine/Engine.h"
namespace HS_Engine
{
	Shader::Shader(std::initializer_list<std::pair<E_ShaderType, std::wstring>> shaders, bool is_mesh_shader)
	{
		auto& context = Engine::Instance().GetWindow()->GetWindowData().m_context;
		auto* device = context->device.Get();
		// compileshader
		for (auto& shader : shaders)
		{
			ComplieShader(shader);
		}

		// TODO : make standard form
	}

	void Shader::MeshShaderInit()
	{
		HRESULT hr;

		D3DX12_MESH_SHADER_PIPELINE_STATE_DESC pipeline_state_desc = {};
		for (auto& shader : mShaderProgram)
		{
			switch (shader.first)
			{
			case E_ShaderType::MESH_SHADER:
				pipeline_state_desc.MS = { reinterpret_cast<UINT8*>(shader.second->GetBufferPointer(), shader.second->GetBufferSize()) };
			case E_ShaderType::PIXEL_SHADER:
				pipeline_state_desc.PS = { reinterpret_cast<UINT8*>(shader.second->GetBufferPointer()), shader.second->GetBufferSize() };
				break;
			default:;
			}
		}

		pipeline_state_desc.pRootSignature = rootSignature.Get();
		pipeline_state_desc.NumRenderTargets = 1;
		pipeline_state_desc.RTVFormats[0] = m_renderTargets[0]->GetDesc().Format;
		pipeline_state_desc.DSVFormat = m_depthStencil->GetDesc().Format;
		pipeline_state_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);    // CW front; cull back
		pipeline_state_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);         // Opaque
		pipeline_state_desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // Less-equal depth test w/ writes; no stencil
		pipeline_state_desc.SampleMask = UINT_MAX;
		pipeline_state_desc.SampleDesc = DefaultSampleDesc();

		auto mesh_pipeline_mesh_state_stream = CD3DX12_PIPELINE_MESH_STATE_STREAM(pipeline_state_desc);

		D3D12_PIPELINE_STATE_STREAM_DESC stream_desc = {};
		stream_desc.SizeInBytes = sizeof(mesh_pipeline_mesh_state_stream);
		stream_desc.pPipelineStateSubobjectStream = &mesh_pipeline_mesh_state_stream;


		const auto& context = Engine::Instance().GetWindow()->GetWindowData().m_context;
		auto* device = context->device.Get();

		hr = device->CreatePipelineState(&stream_desc, IID_PPV_ARGS(&pipelineStateObject));
		if (FAILED(hr))
		{
			//TODO : error handling
		}
	}

	void Shader::DefaultShaderInit()
	{
		HRESULT hr;
		const auto& context = Engine::Instance().GetWindow()->GetWindowData().m_context;
		auto* device = context->device.Get();

		//TODO : dynamic root signature or initalize the root signature parameter
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;

		hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
		if (FAILED(hr))
		{
			//TODO error handling
		}

		hr = device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
		if (FAILED(hr))
		{
			//TODO error Handling
		}

		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeline_state_desc = {};
		pipeline_state_desc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
		pipeline_state_desc.pRootSignature = rootSignature.Get();


		for (auto& shader : mShaderProgram)
		{
			switch (shader.first)
			{
			case E_ShaderType::VERTEX_SHADER:
				pipeline_state_desc.VS = { reinterpret_cast<UINT8*>(shader.second->GetBufferPointer()), shader.second->GetBufferSize() };
				break;
			case E_ShaderType::COMPUTE_SHADER:
				//compute shader will be separately
				break;
			case E_ShaderType::DOMAIN_SHADER:
				pipeline_state_desc.DS = { reinterpret_cast<UINT8*>(shader.second->GetBufferPointer()), shader.second->GetBufferSize() };
				break;
			case E_ShaderType::GEOMETRY_SHADER:
				pipeline_state_desc.GS = { reinterpret_cast<UINT8*>(shader.second->GetBufferPointer()), shader.second->GetBufferSize() };
				break;
			case E_ShaderType::HULL_SHADER:
				pipeline_state_desc.HS = { reinterpret_cast<UINT8*>(shader.second->GetBufferPointer()), shader.second->GetBufferSize() };
				break;
			case E_ShaderType::PIXEL_SHADER:
				pipeline_state_desc.PS = { reinterpret_cast<UINT8*>(shader.second->GetBufferPointer()), shader.second->GetBufferSize() };
				break;
			default:;
			}
		}

		pipeline_state_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		pipeline_state_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		pipeline_state_desc.DepthStencilState.DepthEnable = FALSE;
		pipeline_state_desc.DepthStencilState.StencilEnable = FALSE;
		pipeline_state_desc.SampleMask = UINT_MAX;
		pipeline_state_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		//if we want to deferred(multi render target), change this
		pipeline_state_desc.NumRenderTargets = 1;
		pipeline_state_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		pipeline_state_desc.SampleDesc.Count = 1;



		hr = device->CreateGraphicsPipelineState(&pipeline_state_desc, IID_PPV_ARGS(&pipelineStateObject));
		if (FAILED(hr))
		{
			//TODO : error handling
		}


	}

	void Shader::ComplieShader(std::pair<E_ShaderType,std::wstring> path)
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
			hr = D3DCompileFromFile(path.second.c_str(), nullptr, nullptr, "VSMain", "vs_6_5", compileFlags, 0, &shader_code, nullptr);
			break;
		case E_ShaderType::COMPUTE_SHADER:
			hr = D3DCompileFromFile(path.second.c_str(), nullptr, nullptr, "VSMain", "vs_6_5", compileFlags, 0, &shader_code, nullptr);
			break;
		case E_ShaderType::DOMAIN_SHADER:
			
			break;
		case E_ShaderType::GEOMETRY_SHADER:
			
			break;
		case E_ShaderType::HULL_SHADER:
			
			break;
		case E_ShaderType::PIXEL_SHADER:
			hr = D3DCompileFromFile(path.second.c_str(), nullptr, nullptr, "PSMain", "ps_6_5", compileFlags, 0, &shader_code, nullptr);
			break;
		case E_ShaderType::MESH_SHADER:
			hr = D3DCompileFromFile(path.second.c_str(), nullptr, nullptr, "MSMain", "ms_6_5", compileFlags, 0, &shader_code, nullptr);
		default:;
		}
		if(FAILED(hr))
		{
			//TODO : Error Handling
		}
		mShaderProgram.push_back({ path.first, shader_code });

	}

}

