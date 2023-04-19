#include "Context.h"
#include "d3dx12.h"
#include "imgui.h"

#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx12.h"

HS_Engine::Context::Context()
{
	
}

void HS_Engine::Context::Init()
{
	HRESULT hr;
#if defined(_DEBUG)
	// Enable the D3D12 debug layer.
	{
		Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
		}
	}
#endif
	
	hr = CreateDXGIFactory(IID_PPV_ARGS(&dxigiFactory));
	if(FAILED(hr))
	{
		throw std::runtime_error("Cannot make the DXGIFactory!");
	}

	IDXGIAdapter1* adaptor = nullptr;
	int adaptor_index = 0;
	bool adaptor_is_found = false;


	// find the adpator GPU
	while(dxigiFactory->EnumAdapters1(adaptor_index, &adaptor) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC1 desc;
		adaptor->GetDesc1(&desc);

		if(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			//NO software device
			adaptor_index++;
			continue;
		}

		hr = D3D12CreateDevice(adaptor, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
		if(SUCCEEDED(hr))
		{
			adaptor_is_found = true;
			break;
		}
		adaptor_index++;
	}

	if(!adaptor_is_found)
	{
		MessageBox(NULL, L"Error adaptor finding",
			L"Error", MB_OK | MB_ICONERROR);
		throw std::runtime_error("Cannot find the adaptor! Check the compatible adaptor!");
	}


	D3D12_COMMAND_QUEUE_DESC command_queue_desc = {};


	hr = device->CreateCommandQueue(&command_queue_desc, IID_PPV_ARGS(&commandQueue));
	if(FAILED(hr))
	{
		MessageBox(NULL, L"Error Create Command Queue",
			L"Error", MB_OK | MB_ICONERROR);
	}


	CreateSwapChain();
	CreateRenderTargetDescriptorHeap();
	CreateCommandAllocatorAndList();
	CreateFenceAndEvent();

	Engine::GetImguiManager().Init();


	hr = commandList->Close();
	ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);


	fenceValue[frameIndex]++;
	hr = commandQueue->Signal(fence[frameIndex].Get(), fenceValue[frameIndex]);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Error signaling fence",
					L"Error", MB_OK | MB_ICONERROR);
	}

	
}

void HS_Engine::Context::Update()
{



	Engine::GetImguiManager().Update();
}

void HS_Engine::Context::Render()
{
	HRESULT hr;
	WaitForPreviousFrame();
	UpdatePipeline();

	ID3D12CommandList* pp_command_lists[] = { commandList.Get() };

	commandQueue->ExecuteCommandLists(_countof(pp_command_lists), pp_command_lists);

	hr = commandQueue->Signal(fence[frameIndex].Get(), fenceValue[frameIndex]);
	if(FAILED(hr))
	{
		MessageBox(NULL, L"Error command queue signal",
			L"Error", MB_OK | MB_ICONERROR);
	}

	hr = swapChain->Present(0, 0);
	if(FAILED(hr))
	{
		MessageBox(NULL, L"Error present the current backbuffer",
			L"Error", MB_OK | MB_ICONERROR);
	}

	
}

void HS_Engine::Context::UpdatePipeline()
{
	HRESULT hr;

	hr = commandAllocator[frameIndex]->Reset();
	if(FAILED(hr))
	{
		MessageBox(NULL, L"Error  commandAllocator frameIndex Reset",
			L"Error", MB_OK | MB_ICONERROR);
	}

	hr = commandList->Reset(commandAllocator[frameIndex].Get(), NULL);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Error  commandList frameIndex Reset",
			L"Error", MB_OK | MB_ICONERROR);
	}

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->ResourceBarrier(1, &barrier);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize);

	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	const float clear_color[] = { 0.f,0.2f,0.4f,1.f };
	commandList->ClearRenderTargetView(rtvHandle, clear_color, 0, nullptr);

	ID3D12DescriptorHeap* descriptor_heaps[] = { srvDescriptorHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(descriptor_heaps), descriptor_heaps);

	//imgui render
	Engine::GetImguiManager().Render();

	barrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	commandList->ResourceBarrier(1, &barrier);;

	hr = commandList->Close();

	if(FAILED(hr))
	{
		MessageBox(NULL, L"Error command list close",
			L"Error", MB_OK | MB_ICONERROR);
	}

}

void HS_Engine::Context::WaitForPreviousFrame()
{
	HRESULT hr;

	frameIndex = swapChain->GetCurrentBackBufferIndex();
	if(fence[frameIndex]->GetCompletedValue() < fenceValue[frameIndex])
	{
		hr = fence[frameIndex]->SetEventOnCompletion(fenceValue[frameIndex], fenceEvent);
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Error SetEventOnCompletion",
				L"Error", MB_OK | MB_ICONERROR);
		}
		WaitForSingleObject(fenceEvent, INFINITE);
	}

	fenceValue[frameIndex]++;

}

void HS_Engine::Context::CleanUp()
{
		// wait for the gpu to finish all frames
		for (int i = 0; i < frameBufferCount; ++i)
		{
			frameIndex = i;
			WaitForPreviousFrame();
		}

}


void HS_Engine::Context::CreateSwapChain()
{
	DXGI_MODE_DESC back_buffer_desc = {};
	auto* window = Engine::Instance().GetWindow();
	auto& hwnd = window->GetWindowData().m_hwnd;
	RECT client_rect;
	GetClientRect(hwnd, &client_rect);

	UINT width = client_rect.right - client_rect.left;
	UINT height = client_rect.bottom - client_rect.top;


	back_buffer_desc.Width = width;
	back_buffer_desc.Height = height;
	back_buffer_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	DXGI_SAMPLE_DESC sample_desc = {};
	sample_desc.Count = 1;

	DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
	swap_chain_desc.BufferCount  = frameBufferCount;
	swap_chain_desc.BufferDesc   = back_buffer_desc;
	swap_chain_desc.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.SwapEffect   = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swap_chain_desc.OutputWindow = hwnd;
	swap_chain_desc.SampleDesc   = sample_desc;
	swap_chain_desc.Windowed     = !window->GetWindowData().m_isFullScreen;


	IDXGISwapChain* temp_swap_chain;

	HRESULT hr = dxigiFactory->CreateSwapChain(commandQueue.Get(), &swap_chain_desc, &temp_swap_chain);

	if(FAILED(hr))
	{
		MessageBox(NULL, L"Error Create swapchain",
			L"Error", MB_OK | MB_ICONERROR);
	}
	swapChain = static_cast<IDXGISwapChain3*>(temp_swap_chain);
	frameIndex = swapChain->GetCurrentBackBufferIndex();

}

void HS_Engine::Context::CreateRenderTargetDescriptorHeap()
{
	HRESULT hr;

	//render target view heap
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {};
		rtv_heap_desc.NumDescriptors = frameBufferCount;
		rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		hr = device->CreateDescriptorHeap(&rtv_heap_desc, IID_PPV_ARGS(&rtvDescriptorHeap));

		if (FAILED(hr))
		{
			MessageBox(NULL, L"Error Create RTV descriptor heap!",
			L"Error", MB_OK | MB_ICONERROR);
			throw std::runtime_error("Error Create RTV descriptor heap!");
		}

		rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	//Shader resource view heap
	{
		D3D12_DESCRIPTOR_HEAP_DESC srv_heap_desc = {};
		srv_heap_desc.NumDescriptors = 1000;
		srv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		hr = device->CreateDescriptorHeap(&srv_heap_desc, IID_PPV_ARGS(&srvDescriptorHeap));

		if (FAILED(hr))
		{
			MessageBox(NULL, L"Error Create SRV descriptor heap!",
				L"Error", MB_OK | MB_ICONERROR);
			throw std::runtime_error("Error Create SRV descriptor heap!");
		}

		srvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	


	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	for (int i = 0; i < frameBufferCount; ++i)
	{
		hr = swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]));
		if(FAILED(hr))
		{
			MessageBox(NULL, L"Error Create RTV for each buffer!",
				L"Error", MB_OK | MB_ICONERROR);
		}
		device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, rtvDescriptorSize);
	}

}


void HS_Engine::Context::CreateCommandAllocatorAndList()
{
	HRESULT hr;
	for (int i = 0; i < frameBufferCount; ++i)
	{
		hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator[i]));
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Error Create command allocator!",
				L"Error", MB_OK | MB_ICONERROR);
		}
	}
	//Create CommandList

		hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator[0].Get(), NULL, IID_PPV_ARGS(&commandList));
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Error Create command list!",
				L"Error", MB_OK | MB_ICONERROR);
		}
}


void HS_Engine::Context::CreateFenceAndEvent()
{
	HRESULT hr;
	for(int i = 0; i < frameBufferCount; ++i)
	{
		hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence[i]));
		if(FAILED(hr))
		{
			MessageBox(NULL, L"Error Create Fence!",
				L"Error", MB_OK | MB_ICONERROR);
		}
		fenceValue[i] = 0; 
	}

	fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if(fenceEvent == nullptr)
	{
		MessageBox(NULL, L"Error Create Fence event!",
			L"Error", MB_OK | MB_ICONERROR);
	}
	//WaitForPreviousFrame();
}

