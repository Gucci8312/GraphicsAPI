#include "DirectX12Wrapper.h"

// ����������
HRESULT DirectX12Wrapper::Create(HWND hwnd, RECT rc)
{
	HRESULT hr;

	// �f�o�b�O
#if defined(DEBUG) || defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> Debug;
		hr = D3D12GetDebugInterface(IID_PPV_ARGS(Debug.ReleaseAndGetAddressOf()));

		// �f�o�b�O���C���[�L����
		if (SUCCEEDED(hr))
		{
			Debug->EnableDebugLayer();
		}
	}
#endif

	// �f�o�C�X����
	hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(Dx12Device.ReleaseAndGetAddressOf()));
	if (FAILED(hr)) return hr;

	// �R�}���h�L���[�ݒ�
	D3D12_COMMAND_QUEUE_DESC QueueDesc = {};
	QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	QueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	QueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	QueueDesc.NodeMask = 0;

	// �R�}���h�L���[����
	hr = Dx12Device->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(m_CmdQueue.ReleaseAndGetAddressOf()));
	if (FAILED(hr)) return hr;

	// DXGI�t�@�N�g������
	ComPtr<IDXGIFactory4> Factory = nullptr;
	hr = CreateDXGIFactory1(IID_PPV_ARGS(Factory.ReleaseAndGetAddressOf()));
	if (FAILED(hr)) return hr;

	// �X���b�v�`�F�C���ݒ�
	DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};
	SwapChainDesc.BufferDesc.Width = rc.right;
	SwapChainDesc.BufferDesc.Height = rc.bottom;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.BufferCount = m_FrameCount;
	SwapChainDesc.OutputWindow = hwnd;
	SwapChainDesc.Windowed = TRUE;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// �X���b�v�`�F�C������
	ComPtr<IDXGISwapChain> pSwapChain;
	hr = Factory->CreateSwapChain(m_CmdQueue.Get(), &SwapChainDesc, pSwapChain.ReleaseAndGetAddressOf());
	if (FAILED(hr))	return hr;


	// IDXGISwapChain3���擾
	hr = pSwapChain.As(&m_SwapChain);
	if (FAILED(hr)) return hr;

	// �o�b�N�o�b�t�@�ԍ��擾
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	// ���
	Factory.Reset();
	pSwapChain.Reset();

	// �R�}���h�A���P�[�^����
	for (auto i = 0; i < m_FrameCount; ++i)
	{
		hr = Dx12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_CmdAllocator[i].ReleaseAndGetAddressOf()));
		if (FAILED(hr)) return hr;
	}

	// �R�}���h���X�g����
	hr = Dx12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CmdAllocator[m_FrameIndex].Get(),
		nullptr, IID_PPV_ARGS(m_CmdList.ReleaseAndGetAddressOf()));
	if (FAILED(hr)) return hr;

	// RTV�ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC RTVHeapDesc = {};
	RTVHeapDesc.NumDescriptors = m_FrameCount;
	RTVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	RTVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	RTVHeapDesc.NodeMask = 0;

	// �f�B�X�N���v�^�q�[�v����
	hr = Dx12Device->CreateDescriptorHeap(&RTVHeapDesc, IID_PPV_ARGS(m_HeapRTV.ReleaseAndGetAddressOf()));
	if (FAILED(hr))	return hr;


	auto handle = m_HeapRTV->GetCPUDescriptorHandleForHeapStart();
	auto incrementSize = Dx12Device
		->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	for (auto i = 0u; i < m_FrameCount; ++i)
	{
		hr = m_SwapChain->GetBuffer(i, IID_PPV_ARGS(m_ColorBuffer[i].ReleaseAndGetAddressOf()));
		if (FAILED(hr))	return hr;


		D3D12_RENDER_TARGET_VIEW_DESC viewDesc = {};
		viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;
		viewDesc.Texture2D.PlaneSlice = 0;

		// �����_�[�^�[�Q�b�g�r���[����
		Dx12Device->CreateRenderTargetView(m_ColorBuffer[i].Get(), &viewDesc, handle);

		m_HandleRTV[i] = handle;
		handle.ptr += incrementSize;
	}

	// �t�F���X�J�E���^�[���Z�b�g
	for (auto i = 0u; i < m_FrameCount; ++i)
	{
		m_FenceCounter[i] = 0;
	}

	// �t�F���X�̐���.
	hr = Dx12Device->CreateFence(
		m_FenceCounter[m_FrameIndex],
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(m_Fence.ReleaseAndGetAddressOf()));
	if (FAILED(hr)) return hr;

	m_FenceCounter[m_FrameIndex]++;

	// �C�x���g����
	m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_FenceEvent == nullptr) return hr;

	// �R�}���h���X�g�I��
	m_CmdList->Close();

	// �r���[�|�[�g
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;
	m_ViewPort.Width = static_cast<float>(rc.right);
	m_ViewPort.Height = static_cast<float>(rc.bottom);
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;

	// �V�U�[��`
	m_Scissor.left = 0;
	m_Scissor.right = rc.right;
	m_Scissor.top = 0;
	m_Scissor.bottom = rc.bottom;

	// �[�x�X�e���V���o�b�t�@�ݒ�
	D3D12_HEAP_PROPERTIES DepthHeapProp = {};
	DepthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	DepthHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	DepthHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	DepthHeapProp.CreationNodeMask = 1;
	DepthHeapProp.VisibleNodeMask = 1;

	// �[�x�p���\�[�X�ݒ�
	D3D12_RESOURCE_DESC DepthResDesc = {};
	DepthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	DepthResDesc.Alignment = 0;
	DepthResDesc.Width = rc.right;
	DepthResDesc.Height = rc.bottom;
	DepthResDesc.DepthOrArraySize = 1;
	DepthResDesc.MipLevels = 1;
	DepthResDesc.Format = DXGI_FORMAT_D32_FLOAT;
	DepthResDesc.SampleDesc.Count = 1;
	DepthResDesc.SampleDesc.Quality = 0;
	DepthResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	DepthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	// �[�x�o�b�t�@�N���A�l
	D3D12_CLEAR_VALUE ClearValue;
	ClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	ClearValue.DepthStencil.Depth = 1.0f;
	ClearValue.DepthStencil.Stencil = 0;


	// �[�x�o�b�t�@����
	hr = Dx12Device->CreateCommittedResource(&DepthHeapProp, D3D12_HEAP_FLAG_NONE, &DepthResDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &ClearValue, IID_PPV_ARGS(m_DepthBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr)) return hr;

	// �f�B�X�N���v�^�q�[�v�ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC DepthHeapDesc = {};
	DepthHeapDesc.NumDescriptors = 1;
	DepthHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DepthHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DepthHeapDesc.NodeMask = 0;

	// �f�B�X�N���v�^�q�[�v����
	hr = Dx12Device->CreateDescriptorHeap(&DepthHeapDesc, IID_PPV_ARGS(m_HeapDSV.ReleaseAndGetAddressOf()));
	if (FAILED(hr)) return hr;

	handle = m_HeapDSV->GetCPUDescriptorHandleForHeapStart();
	incrementSize = Dx12Device
		->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	D3D12_DEPTH_STENCIL_VIEW_DESC DepthViewDesc = {};
	DepthViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	DepthViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	DepthViewDesc.Texture2D.MipSlice = 0;
	DepthViewDesc.Flags = D3D12_DSV_FLAG_NONE;

	// DSV����
	Dx12Device->CreateDepthStencilView(m_DepthBuffer.Get(), &DepthViewDesc, handle);

	m_HandleDSV = handle;

	return S_OK;
}

// �I������
void DirectX12Wrapper::Release()
{
	// �t�F���X����
	WaitGPU();
	Dx12Device.Reset();
	m_CmdQueue.Reset();
	m_SwapChain.Reset();
	m_CmdList.Reset();
	m_HeapRTV.Reset();
	m_HeapDSV.Reset();
	m_Fence.Reset();
	m_DepthBuffer.Reset();
	for (auto Idx = 0; Idx < m_FrameCount; ++Idx)
	{
		m_CmdAllocator[Idx].Reset();
		m_ColorBuffer[Idx].Reset();
	}

	// �C�x���g�j��
	if (m_FenceEvent != nullptr)
	{
		CloseHandle(m_FenceEvent);
		m_FenceEvent = nullptr;
	}
}

// �t�F���X����
void DirectX12Wrapper::WaitGPU()
{
	assert(m_CmdQueue != nullptr);
	assert(m_Fence != nullptr);
	assert(m_FenceEvent != nullptr);

	// �V�O�i��
	m_CmdQueue->Signal(m_Fence.Get(), m_FenceCounter[m_FrameIndex]);

	// �������ɃC�x���g�ݒ�
	m_Fence->SetEventOnCompletion(m_FenceCounter[m_FrameIndex], m_FenceEvent);

	// �ҋ@����
	WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);

	// �J�E���^�[����
	m_FenceCounter[m_FrameIndex]++;
}

// �`��O����
void DirectX12Wrapper::BeforeRender()
{
	// �R�}���h���͊J�n
	m_CmdAllocator[m_FrameIndex]->Reset();
	m_CmdList->Reset(m_CmdAllocator[m_FrameIndex].Get(), nullptr);

	SetResouceBarrier(m_ColorBuffer[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// �����_�[�Q�b�g�ݒ�
	m_CmdList->OMSetRenderTargets(1, &m_HandleRTV[m_FrameIndex], false, &m_HandleDSV);

	// �N���A�J���[�ݒ�
	float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	// �����_�[�^�[�Q�b�g�r���[���N���A
	m_CmdList->ClearRenderTargetView(m_HandleRTV[m_FrameIndex], clearColor, 0, nullptr);

	// �[�x�X�e���V���r���[���N���A
	m_CmdList->ClearDepthStencilView(m_HandleDSV, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	m_CmdList->RSSetViewports(1, &m_ViewPort);
	m_CmdList->RSSetScissorRects(1, &m_Scissor);

}

// �`��㏈��
void DirectX12Wrapper::AfterRender()
{
	SetResouceBarrier(m_ColorBuffer[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	// �R�}���h�̋L�^���I��
	m_CmdList->Close();

	// �R�}���h�����s
	ID3D12CommandList* ppCmdLists[] = { m_CmdList.Get() };
	m_CmdQueue->ExecuteCommandLists(1, ppCmdLists);


	// ��ʂɕ\��
	m_SwapChain->Present(1, 0);

	// �V�O�i������
	const auto currentValue = m_FenceCounter[m_FrameIndex];
	m_CmdQueue->Signal(m_Fence.Get(), currentValue);

	// �o�b�N�o�b�t�@�ԍ����X�V
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	// �t�F���X����
	if (m_Fence->GetCompletedValue() < m_FenceCounter[m_FrameIndex])
	{
		m_Fence->SetEventOnCompletion(m_FenceCounter[m_FrameIndex], m_FenceEvent);
		WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);
	}

	// �t���[���J�E���^�[����
	m_FenceCounter[m_FrameIndex] = currentValue + 1;
}

// ���\�[�X�o���A
void DirectX12Wrapper::SetResouceBarrier(ID3D12Resource* Resouce, D3D12_RESOURCE_STATES Before, D3D12_RESOURCE_STATES After)
{
	// ���\�[�X�o���A�ݒ�
	D3D12_RESOURCE_BARRIER BarrierDesc;
	ZeroMemory(&BarrierDesc, sizeof(BarrierDesc));
	BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	BarrierDesc.Transition.pResource = Resouce;
	BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	BarrierDesc.Transition.StateBefore = Before;
	BarrierDesc.Transition.StateAfter = After;

	// ���\�[�X�o���A�Z�b�g
	m_CmdList->ResourceBarrier(1, &BarrierDesc);
}
