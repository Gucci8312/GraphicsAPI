#pragma once

#include "DirectXAllRapper.h"

// DirectX12èâä˙âªÉNÉâÉX
class DirectX12Wrapper : public DirectXAllRapper
{
private:
	static const uint32_t				   m_FrameCount = 2;
	//ComPtr<ID3D12Device>                   m_Device;
	ComPtr<ID3D12CommandQueue>             m_CmdQueue;
	ComPtr<IDXGISwapChain3>                m_SwapChain;
	ComPtr<ID3D12CommandAllocator>         m_CmdAllocator[m_FrameCount];
	ComPtr<ID3D12GraphicsCommandList>      m_CmdList;
	ComPtr<ID3D12DescriptorHeap>           m_HeapRTV;
	ComPtr<ID3D12DescriptorHeap>           m_HeapDSV;
	ComPtr<ID3D12Resource>                 m_ColorBuffer[m_FrameCount];
	ComPtr<ID3D12Fence>                    m_Fence;
	ComPtr<ID3D12Resource>				   m_DepthBuffer;
	HANDLE								   m_FenceEvent = {};
	uint64_t							   m_FenceCounter[m_FrameCount];
	uint32_t                               m_FrameIndex = 0;
	D3D12_CPU_DESCRIPTOR_HANDLE            m_HandleRTV[m_FrameCount];
	D3D12_CPU_DESCRIPTOR_HANDLE            m_HandleDSV = {};
	D3D12_VIEWPORT                         m_ViewPort = {};
	D3D12_RECT                             m_Scissor = {};


public:
	HRESULT Create(HWND hwnd, RECT rc)	override final;
	void    Release()					override final;
	void	BeforeRender()				override final;
	void	AfterRender()				override final;
	
	void	SetResouceBarrier(ID3D12Resource* Resouce, D3D12_RESOURCE_STATES Before, D3D12_RESOURCE_STATES After);
	void	WaitGPU();

	
	ID3D12GraphicsCommandList* GetCmdList()
	{
		return m_CmdList.Get();
	}
	uint32_t GetFrameIndex()
	{
		return m_FrameIndex;
	}
};

