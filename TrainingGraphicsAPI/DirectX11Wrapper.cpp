#include "DirectX11Wrapper.h"

// 初期化
HRESULT DirectX11Wrapper::Create(HWND hwnd, RECT rc)
{
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes = 0;
	size_t stringLength;
	DXGI_ADAPTER_DESC adapterDesc;

	// グラフィックインタフェースファクトリを作成
	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(hr)) return hr;

	int GPUNumber = 0;
	int GPUMaxMem = 0;

	// GPUアダプタを検索
	for (int i = 0; i < 100; i++)
	{
		IDXGIAdapter* add;
		if (FAILED(factory->EnumAdapters(i, &add)))
			break;
		hr = add->GetDesc(&adapterDesc);

		char videoCardDescription[128];
		// ビデオカード名を取得
		int error = wcstombs_s(&stringLength, videoCardDescription, 128, adapterDesc.Description, 128);
		if (error != 0)
		{
			break;
		}

		// ビデオカードメモリを取得（MB単位）
		int videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

		// アウトプット（モニター）に番号IDを付ける
		if (FAILED(add->EnumOutputs(0, &adapterOutput)))
		{
			continue;
		}

		// DXGI_FORMAT_R8G8B8A8_UNORM の表示形式数を取得する
		if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL)))
		{
			continue;
		}

		if (videoCardMemory > GPUMaxMem)
		{
			GPUMaxMem = videoCardMemory;
			GPUNumber = i;
		}
		add->Release();

		// アダプタアウトプットを解放
		adapterOutput->Release();
		adapterOutput = 0;
	}

	// グラフィックインタフェースアダプターを作成
	if (FAILED(factory->EnumAdapters(GPUNumber, &adapter)))
	{
		return E_FAIL;
	}

	UINT cdev_flags = 0;
#ifdef _DEBUG
	cdev_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// スワップチェイン設定
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = rc.right;
	sd.BufferDesc.Height = rc.bottom;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hwnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = true;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
	};

	// DirectX11デバイスとスワップチェイン作成
	if (FAILED(D3D11CreateDeviceAndSwapChain(adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, cdev_flags,
		featureLevels, 1, D3D11_SDK_VERSION, &sd, &m_SwapChain, &m_Device, NULL, &m_ImmediateContext)))
	{
		return E_FAIL;
	}

	// アダプタを解放
	adapter->Release();
	adapter = 0;

	// ファクトリを解放
	factory->Release();
	factory = 0;

	ComPtr<ID3D11Texture2D> pBackBuffer;
	// スワップチェインに用意されたバッファ（2Dテクスチャ）を取得
	if (FAILED(m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)))) {
		return E_FAIL;
	}

	// レンダーターゲットView作成
	if (FAILED(m_Device->CreateRenderTargetView(pBackBuffer.Get(), NULL, &m_RenderTargetView))) {
		return hr;
	}

	// 深度ステンシルバッファ作成
	D3D11_TEXTURE2D_DESC txDesc = {};
	txDesc.Width = rc.right;
	txDesc.Height = rc.bottom;
	txDesc.MipLevels = 1;
	txDesc.ArraySize = 1;
	txDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	txDesc.SampleDesc.Count = 1;
	txDesc.SampleDesc.Quality = 0;
	txDesc.Usage = D3D11_USAGE_DEFAULT;
	txDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	txDesc.CPUAccessFlags = 0;
	txDesc.MiscFlags = 0;

	if (FAILED(m_Device->CreateTexture2D(&txDesc, NULL, m_DepthStencilTexture.ReleaseAndGetAddressOf())))
	{
		return E_FAIL;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc = {};
	dsDesc.Format = txDesc.Format;
	dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsDesc.Texture2D.MipSlice = 0;

	if (FAILED(m_Device->CreateDepthStencilView(m_DepthStencilTexture.Get(), &dsDesc, m_DepthStencilView.ReleaseAndGetAddressOf())))
	{
		return E_FAIL;
	}

	// ラスタライザー設定
	D3D11_RASTERIZER_DESC RasterDesc;
	RasterDesc.AntialiasedLineEnable = false;
	RasterDesc.CullMode = D3D11_CULL_BACK;
	RasterDesc.DepthBias = 0;
	RasterDesc.DepthBiasClamp = 0.0f;
	RasterDesc.DepthClipEnable = true;
	RasterDesc.FillMode = D3D11_FILL_SOLID;
	RasterDesc.FrontCounterClockwise = false;
	RasterDesc.MultisampleEnable = false;
	RasterDesc.ScissorEnable = false;
	RasterDesc.SlopeScaledDepthBias = 0.0f;

	// ラスタライザー生成
	hr = m_Device->CreateRasterizerState(&RasterDesc, m_RasterState.ReleaseAndGetAddressOf());
	if (FAILED(hr)) return hr;

	// ビューポート設定
	m_ViewPort.Width = static_cast<FLOAT>(rc.right - rc.left);
	m_ViewPort.Height = static_cast<FLOAT>(rc.bottom - rc.top);
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;


	return S_OK;
}

// 終了処理
void DirectX11Wrapper::Release()
{
	m_Device.Reset();
	m_ImmediateContext.Reset();
	m_SwapChain.Reset();
	m_RenderTargetView.Reset();
	m_DepthStencilTexture.Reset();
	m_DepthStencilView.Reset();
	m_RasterState.Reset();
}

// 描画前処理
void DirectX11Wrapper::BeforeRender()
{
	// 画面リセット
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_ImmediateContext->ClearRenderTargetView(m_RenderTargetView.Get(), clearColor);
	m_ImmediateContext->ClearDepthStencilView(m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_ImmediateContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_DepthStencilView.Get());
	m_ImmediateContext->RSSetState(m_RasterState.Get());
	m_ImmediateContext->RSSetViewports(1, &m_ViewPort);
}

// 描画後処理
void DirectX11Wrapper::AfterRender()
{
	m_SwapChain->Present(0, 0);
}

