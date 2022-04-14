#include "DirectX12Wrapper.h"

// 初期化処理
HRESULT DirectX12Wrapper::Create(HWND hwnd, RECT rc)
{
	HRESULT hr;

	// デバッグ
#if defined(DEBUG) || defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> Debug;
		hr = D3D12GetDebugInterface(IID_PPV_ARGS(Debug.ReleaseAndGetAddressOf()));

		// デバッグレイヤー有効化
		if (SUCCEEDED(hr))
		{
			Debug->EnableDebugLayer();
		}
	}
#endif

	// デバイス生成
	hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(Dx12Device.ReleaseAndGetAddressOf()));
	if (FAILED(hr)) return hr;

	// コマンドキュー設定
	D3D12_COMMAND_QUEUE_DESC QueueDesc = {};
	QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	QueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	QueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	QueueDesc.NodeMask = 0;

	// コマンドキュー生成
	hr = Dx12Device->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(m_CmdQueue.ReleaseAndGetAddressOf()));
	if (FAILED(hr)) return hr;

	// DXGIファクトリ生成
	ComPtr<IDXGIFactory4> Factory = nullptr;
	hr = CreateDXGIFactory1(IID_PPV_ARGS(Factory.ReleaseAndGetAddressOf()));
	if (FAILED(hr)) return hr;

	// スワップチェイン設定
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

	// スワップチェイン生成
	ComPtr<IDXGISwapChain> pSwapChain;
	hr = Factory->CreateSwapChain(m_CmdQueue.Get(), &SwapChainDesc, pSwapChain.ReleaseAndGetAddressOf());
	if (FAILED(hr))	return hr;


	// IDXGISwapChain3を取得
	hr = pSwapChain.As(&m_SwapChain);
	if (FAILED(hr)) return hr;

	// バックバッファ番号取得
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	// 解放
	Factory.Reset();
	pSwapChain.Reset();

	// コマンドアロケータ生成
	for (auto i = 0; i < m_FrameCount; ++i)
	{
		hr = Dx12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_CmdAllocator[i].ReleaseAndGetAddressOf()));
		if (FAILED(hr)) return hr;
	}

	// コマンドリスト生成
	hr = Dx12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CmdAllocator[m_FrameIndex].Get(),
		nullptr, IID_PPV_ARGS(m_CmdList.ReleaseAndGetAddressOf()));
	if (FAILED(hr)) return hr;

	// RTV設定
	D3D12_DESCRIPTOR_HEAP_DESC RTVHeapDesc = {};
	RTVHeapDesc.NumDescriptors = m_FrameCount;
	RTVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	RTVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	RTVHeapDesc.NodeMask = 0;

	// ディスクリプタヒープ生成
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

		// レンダーターゲットビュー生成
		Dx12Device->CreateRenderTargetView(m_ColorBuffer[i].Get(), &viewDesc, handle);

		m_HandleRTV[i] = handle;
		handle.ptr += incrementSize;
	}

	// フェンスカウンターリセット
	for (auto i = 0u; i < m_FrameCount; ++i)
	{
		m_FenceCounter[i] = 0;
	}

	// フェンスの生成.
	hr = Dx12Device->CreateFence(
		m_FenceCounter[m_FrameIndex],
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(m_Fence.ReleaseAndGetAddressOf()));
	if (FAILED(hr)) return hr;

	m_FenceCounter[m_FrameIndex]++;

	// イベント生成
	m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_FenceEvent == nullptr) return hr;

	// コマンドリスト終了
	m_CmdList->Close();

	// ビューポート
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;
	m_ViewPort.Width = static_cast<float>(rc.right);
	m_ViewPort.Height = static_cast<float>(rc.bottom);
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;

	// シザー矩形
	m_Scissor.left = 0;
	m_Scissor.right = rc.right;
	m_Scissor.top = 0;
	m_Scissor.bottom = rc.bottom;

	// 深度ステンシルバッファ設定
	D3D12_HEAP_PROPERTIES DepthHeapProp = {};
	DepthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	DepthHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	DepthHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	DepthHeapProp.CreationNodeMask = 1;
	DepthHeapProp.VisibleNodeMask = 1;

	// 深度用リソース設定
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

	// 深度バッファクリア値
	D3D12_CLEAR_VALUE ClearValue;
	ClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	ClearValue.DepthStencil.Depth = 1.0f;
	ClearValue.DepthStencil.Stencil = 0;


	// 深度バッファ生成
	hr = Dx12Device->CreateCommittedResource(&DepthHeapProp, D3D12_HEAP_FLAG_NONE, &DepthResDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &ClearValue, IID_PPV_ARGS(m_DepthBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr)) return hr;

	// ディスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC DepthHeapDesc = {};
	DepthHeapDesc.NumDescriptors = 1;
	DepthHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DepthHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DepthHeapDesc.NodeMask = 0;

	// ディスクリプタヒープ生成
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

	// DSV生成
	Dx12Device->CreateDepthStencilView(m_DepthBuffer.Get(), &DepthViewDesc, handle);

	m_HandleDSV = handle;

	return S_OK;
}

// 終了処理
void DirectX12Wrapper::Release()
{
	// フェンス処理
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

	// イベント破棄
	if (m_FenceEvent != nullptr)
	{
		CloseHandle(m_FenceEvent);
		m_FenceEvent = nullptr;
	}
}

// フェンス処理
void DirectX12Wrapper::WaitGPU()
{
	assert(m_CmdQueue != nullptr);
	assert(m_Fence != nullptr);
	assert(m_FenceEvent != nullptr);

	// シグナル
	m_CmdQueue->Signal(m_Fence.Get(), m_FenceCounter[m_FrameIndex]);

	// 完了時にイベント設定
	m_Fence->SetEventOnCompletion(m_FenceCounter[m_FrameIndex], m_FenceEvent);

	// 待機処理
	WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);

	// カウンター増加
	m_FenceCounter[m_FrameIndex]++;
}

// 描画前処理
void DirectX12Wrapper::BeforeRender()
{
	// コマンド入力開始
	m_CmdAllocator[m_FrameIndex]->Reset();
	m_CmdList->Reset(m_CmdAllocator[m_FrameIndex].Get(), nullptr);

	SetResouceBarrier(m_ColorBuffer[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// レンダーゲット設定
	m_CmdList->OMSetRenderTargets(1, &m_HandleRTV[m_FrameIndex], false, &m_HandleDSV);

	// クリアカラー設定
	float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	// レンダーターゲットビューをクリア
	m_CmdList->ClearRenderTargetView(m_HandleRTV[m_FrameIndex], clearColor, 0, nullptr);

	// 深度ステンシルビューをクリア
	m_CmdList->ClearDepthStencilView(m_HandleDSV, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	m_CmdList->RSSetViewports(1, &m_ViewPort);
	m_CmdList->RSSetScissorRects(1, &m_Scissor);

}

// 描画後処理
void DirectX12Wrapper::AfterRender()
{
	SetResouceBarrier(m_ColorBuffer[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	// コマンドの記録を終了
	m_CmdList->Close();

	// コマンドを実行
	ID3D12CommandList* ppCmdLists[] = { m_CmdList.Get() };
	m_CmdQueue->ExecuteCommandLists(1, ppCmdLists);


	// 画面に表示
	m_SwapChain->Present(1, 0);

	// シグナル処理
	const auto currentValue = m_FenceCounter[m_FrameIndex];
	m_CmdQueue->Signal(m_Fence.Get(), currentValue);

	// バックバッファ番号を更新
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	// フェンス処理
	if (m_Fence->GetCompletedValue() < m_FenceCounter[m_FrameIndex])
	{
		m_Fence->SetEventOnCompletion(m_FenceCounter[m_FrameIndex], m_FenceEvent);
		WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);
	}

	// フレームカウンター増加
	m_FenceCounter[m_FrameIndex] = currentValue + 1;
}

// リソースバリア
void DirectX12Wrapper::SetResouceBarrier(ID3D12Resource* Resouce, D3D12_RESOURCE_STATES Before, D3D12_RESOURCE_STATES After)
{
	// リソースバリア設定
	D3D12_RESOURCE_BARRIER BarrierDesc;
	ZeroMemory(&BarrierDesc, sizeof(BarrierDesc));
	BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	BarrierDesc.Transition.pResource = Resouce;
	BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	BarrierDesc.Transition.StateBefore = Before;
	BarrierDesc.Transition.StateAfter = After;

	// リソースバリアセット
	m_CmdList->ResourceBarrier(1, &BarrierDesc);
}
