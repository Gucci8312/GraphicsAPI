#include "DirectX11Wrapper.h"

// ������
HRESULT DirectX11Wrapper::Create(HWND hwnd, RECT rc)
{
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes = 0;
	size_t stringLength;
	DXGI_ADAPTER_DESC adapterDesc;

	// �O���t�B�b�N�C���^�t�F�[�X�t�@�N�g�����쐬
	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(hr)) return hr;

	int GPUNumber = 0;
	int GPUMaxMem = 0;

	// GPU�A�_�v�^������
	for (int i = 0; i < 100; i++)
	{
		IDXGIAdapter* add;
		if (FAILED(factory->EnumAdapters(i, &add)))
			break;
		hr = add->GetDesc(&adapterDesc);

		char videoCardDescription[128];
		// �r�f�I�J�[�h�����擾
		int error = wcstombs_s(&stringLength, videoCardDescription, 128, adapterDesc.Description, 128);
		if (error != 0)
		{
			break;
		}

		// �r�f�I�J�[�h���������擾�iMB�P�ʁj
		int videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

		// �A�E�g�v�b�g�i���j�^�[�j�ɔԍ�ID��t����
		if (FAILED(add->EnumOutputs(0, &adapterOutput)))
		{
			continue;
		}

		// DXGI_FORMAT_R8G8B8A8_UNORM �̕\���`�������擾����
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

		// �A�_�v�^�A�E�g�v�b�g�����
		adapterOutput->Release();
		adapterOutput = 0;
	}

	// �O���t�B�b�N�C���^�t�F�[�X�A�_�v�^�[���쐬
	if (FAILED(factory->EnumAdapters(GPUNumber, &adapter)))
	{
		return E_FAIL;
	}

	UINT cdev_flags = 0;
#ifdef _DEBUG
	cdev_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// �X���b�v�`�F�C���ݒ�
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

	// DirectX11�f�o�C�X�ƃX���b�v�`�F�C���쐬
	if (FAILED(D3D11CreateDeviceAndSwapChain(adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, cdev_flags,
		featureLevels, 1, D3D11_SDK_VERSION, &sd, &m_SwapChain, &m_Device, NULL, &m_ImmediateContext)))
	{
		return E_FAIL;
	}

	// �A�_�v�^�����
	adapter->Release();
	adapter = 0;

	// �t�@�N�g�������
	factory->Release();
	factory = 0;

	ComPtr<ID3D11Texture2D> pBackBuffer;
	// �X���b�v�`�F�C���ɗp�ӂ��ꂽ�o�b�t�@�i2D�e�N�X�`���j���擾
	if (FAILED(m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)))) {
		return E_FAIL;
	}

	// �����_�[�^�[�Q�b�gView�쐬
	if (FAILED(m_Device->CreateRenderTargetView(pBackBuffer.Get(), NULL, &m_RenderTargetView))) {
		return hr;
	}

	// �[�x�X�e���V���o�b�t�@�쐬
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

	// ���X�^���C�U�[�ݒ�
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

	// ���X�^���C�U�[����
	hr = m_Device->CreateRasterizerState(&RasterDesc, m_RasterState.ReleaseAndGetAddressOf());
	if (FAILED(hr)) return hr;

	// �r���[�|�[�g�ݒ�
	m_ViewPort.Width = static_cast<FLOAT>(rc.right - rc.left);
	m_ViewPort.Height = static_cast<FLOAT>(rc.bottom - rc.top);
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;


	return S_OK;
}

// �I������
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

// �`��O����
void DirectX11Wrapper::BeforeRender()
{
	// ��ʃ��Z�b�g
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_ImmediateContext->ClearRenderTargetView(m_RenderTargetView.Get(), clearColor);
	m_ImmediateContext->ClearDepthStencilView(m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_ImmediateContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_DepthStencilView.Get());
	m_ImmediateContext->RSSetState(m_RasterState.Get());
	m_ImmediateContext->RSSetViewports(1, &m_ViewPort);
}

// �`��㏈��
void DirectX11Wrapper::AfterRender()
{
	m_SwapChain->Present(0, 0);
}

