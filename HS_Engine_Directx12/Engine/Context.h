#ifndef CONTEXT_H
#define CONTEXT_H
#include "Engine.h"
#include "stdafx.h"
#pragma warning
using Microsoft::WRL::ComPtr;

namespace HS_Engine
{
	class Context
	{
	public:
		Context();
		void Init();
		void Update();
		void Render();
		void UpdatePipeline();
		void WaitForPreviousFrame();
		void CleanUp();
		
	private:
		void CreateSwapChain();
		void CreateRenderTargetDescriptorHeap();
		void CreateCommandAllocatorAndList();
		void CreateFenceAndEvent();

	public:
		IDXGIFactory4* dxigiFactory = nullptr;
		static const int frameBufferCount = 3; // number of buffers we want, 2 for double buffering, 3 for tripple buffering

		ComPtr<ID3D12Device2> device; // direct3d device

		ComPtr<IDXGISwapChain3> swapChain; // swapchain used to switch between render targets

		ComPtr<ID3D12CommandQueue> commandQueue; // container for command lists

		ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap; // a descriptor heap to hold resources like the render targets

		ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;

		ComPtr<ID3D12Resource> renderTargets[frameBufferCount];
	 // number of render targets equal to buffer countK

		ComPtr <ID3D12CommandAllocator> commandAllocator[frameBufferCount]; // we want enough allocators for each buffer * number of threads (we only have one thread)

		ComPtr <ID3D12GraphicsCommandList> commandList; // a command list we can record commands into, then execute them to render the frame

		ComPtr <ID3D12Fence> fence[frameBufferCount];    // an object that is locked while our command list is being executed by the gpu. We need as many 
		//										 //as we have allocators (more if we want to know when the gpu is finished with an asset)

		HANDLE fenceEvent; // a handle to an event when our fence is unlocked by the gpu

		UINT64 fenceValue[frameBufferCount]; // this value is incremented each frame. each fence will have its own value

		int frameIndex; // current rtv we are on

		int rtvDescriptorSize; // size of the rtv descriptor on the device (all front and back buffers will be the same size)

		int srvDescriptorSize;


		D3D12_VIEWPORT m_viewport;
		D3D12_RECT m_scissorRect;
	};
}


#endif//!CONTEXT_H
