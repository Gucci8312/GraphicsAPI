#include "Object.h"

bool Object::ObjectCreate(ID3D11Device* Device, Vertex* VertexList, int VertexNum, unsigned long* IndexList, int IndexNum, ID3D11DeviceContext* Context)
{
	auto VertexByteSize = sizeof(Vertex) * VertexNum;
	auto IndexByteSize = sizeof(unsigned short) * IndexNum;

	// 描画用インデックス数取得
	m_IndexNum = IndexNum;

	// 頂点バッファ生成
	auto hr = DirectX11Util::CreateBuffer(Device, D3D11_BIND_VERTEX_BUFFER, VertexList, (UINT)VertexByteSize, m_11VertexBuffer.ReleaseAndGetAddressOf());
	if (FAILED(hr)) return false;

	// インデックスバッファ生成
	hr = DirectX11Util::CreateBuffer(Device, D3D11_BIND_INDEX_BUFFER, IndexList, (UINT)IndexByteSize, m_11IndexBuffer.ReleaseAndGetAddressOf());
	if (FAILED(hr)) return false;

	ComPtr<ID3DBlob> pErrorBlob = NULL;

	// 頂点シェーダコンパイル
	ComPtr<ID3DBlob> vsblob;
	CompileShader("Shader/SimpleVS.hlsl", "main", "vs_5_0", vsblob.ReleaseAndGetAddressOf());

	// 頂点シェーダー生成
	hr = Device->CreateVertexShader(vsblob->GetBufferPointer(), vsblob->GetBufferSize(), NULL, m_11VertexShader.ReleaseAndGetAddressOf());
	if (FAILED(hr)) return false;


	// 頂点データ設定
	D3D11_INPUT_ELEMENT_DESC VertexDesc[]
	{
		{"POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT    ,0 ,                            0, D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR"    , 0, DXGI_FORMAT_R32G32B32A32_FLOAT ,0 , D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0},
		{"NORMAL"   , 0, DXGI_FORMAT_R32G32B32_FLOAT    ,0 , D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD" , 0, DXGI_FORMAT_R32G32_FLOAT       ,0 , D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0},
	};

	// インプットレイアウト生成
	hr = Device->CreateInputLayout(VertexDesc, ARRAYSIZE(VertexDesc), vsblob->GetBufferPointer(), vsblob->GetBufferSize(), m_11InputLayOut.ReleaseAndGetAddressOf());
	if (FAILED(hr)) return false;

	// ピクセルシェーダーコンパイル
	ComPtr<ID3DBlob> psblob;
	CompileShader("Shader/TexturePS.hlsl", "main", "ps_5_0", psblob.ReleaseAndGetAddressOf());

	// ピクセルシェーダー生成
	hr = Device->CreatePixelShader(psblob->GetBufferPointer(), psblob->GetBufferSize(), NULL, m_11PixelShader.ReleaseAndGetAddressOf());
	if (FAILED(hr)) return false;

	// 定数バッファ生成
	hr = DirectX11Util::CreateBuffer(Device, D3D11_BIND_CONSTANT_BUFFER, (sizeof(ConstantBuffer) + 0xff) & ~0xff, m_11ConstantBuffer.ReleaseAndGetAddressOf());
	if (FAILED(hr)) return false;

	// テクスチャ生成
	if (CreateTexture(Device, Context)) return false;

	return true;
}

bool Object::ObjectCreate(ID3D12Device* Device, Vertex* VertexList, int VertexNum, unsigned long* IndexList, int IndexNum)
{
	auto VertexByteSize = sizeof(Vertex) * VertexNum;
	auto IndexByteSize = sizeof(unsigned int) * IndexNum;

	auto hr = DirectX12Util::CreateBuffer(Device, (UINT)VertexByteSize, m_12VertexBuffer.ReleaseAndGetAddressOf());
	if (FAILED(hr)) return false;

	// 頂点データ転送開始
	void* SendVertexData = nullptr;
	hr = m_12VertexBuffer->Map(0, nullptr, &SendVertexData);
	if (FAILED(hr)) hr;

	// 転送
	memcpy(SendVertexData, VertexList, VertexByteSize);

	// 頂点データ転送終了
	m_12VertexBuffer->Unmap(0, nullptr);

	// 頂点バッファービュー設定
	m_12VBView.BufferLocation = m_12VertexBuffer->GetGPUVirtualAddress();
	m_12VBView.SizeInBytes = static_cast<UINT>(VertexByteSize);
	m_12VBView.StrideInBytes = static_cast<UINT>(sizeof(Vertex));

	// 描画用インデックス数取得
	m_IndexNum = IndexNum;

	// インデックスバッファ生成
	hr = DirectX12Util::CreateBuffer(Device, (UINT)IndexByteSize, m_12IndexBuffer.ReleaseAndGetAddressOf());
	if (FAILED(hr)) return false;

	// インデックスデータ転送開始
	void* SendIndexData = nullptr;
	hr = m_12IndexBuffer->Map(0, nullptr, &SendIndexData);
	if (FAILED(hr)) return false;

	// 転送
	memcpy(SendIndexData, IndexList, IndexByteSize);

	// インデックスデータ転送終了
	m_12IndexBuffer->Unmap(0, nullptr);

	// インデックスバッファビュー設定
	m_12IBView.BufferLocation = m_12IndexBuffer->GetGPUVirtualAddress();
	m_12IBView.Format = DXGI_FORMAT_R32_UINT;
	m_12IBView.SizeInBytes = (UINT)IndexByteSize;

	// 定数バッファ用ディスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC BasicDesc = {};
	BasicDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	BasicDesc.NumDescriptors = 2;
	BasicDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	BasicDesc.NodeMask = 0;

	// CBV/SRV/UAV用ディスクリプタヒープ生成
	hr = Device->CreateDescriptorHeap(&BasicDesc, IID_PPV_ARGS(m_12BasicDescHeap.GetAddressOf()));
	if (FAILED(hr)) return false;

	// 定数バッファ設定
	D3D12_HEAP_PROPERTIES CBProp = {};
	CBProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	CBProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	CBProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	CBProp.CreationNodeMask = 1;
	CBProp.VisibleNodeMask = 1;

	// リソースの設定
	D3D12_RESOURCE_DESC CBResDesc = {};
	CBResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	CBResDesc.Alignment = 0;
	CBResDesc.Width = (sizeof(ConstantBuffer) + 0xff) & ~0xff;
	CBResDesc.Height = 1;
	CBResDesc.DepthOrArraySize = 1;
	CBResDesc.MipLevels = 1;
	CBResDesc.Format = DXGI_FORMAT_UNKNOWN;
	CBResDesc.SampleDesc.Count = 1;
	CBResDesc.SampleDesc.Quality = 0;
	CBResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	CBResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// インクリメント用サイズ取得
	auto incrementSize = Device
		->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	for (auto Idx = 0; Idx < 2; ++Idx)
	{
		hr = Device->CreateCommittedResource(&CBProp, D3D12_HEAP_FLAG_NONE,
			&CBResDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(m_12ConstantBuffer[Idx].ReleaseAndGetAddressOf()));
		if (FAILED(hr)) return false;

		auto Address = m_12ConstantBuffer[Idx]->GetGPUVirtualAddress();
		auto HandleCPU = m_12BasicDescHeap->GetCPUDescriptorHandleForHeapStart();
		auto HandleGPU = m_12BasicDescHeap->GetGPUDescriptorHandleForHeapStart();

		HandleCPU.ptr += incrementSize * Idx;
		HandleGPU.ptr += incrementSize * Idx;

		// 定数バッファビュー設定
		m_12CBView[Idx].HandleCPU = HandleCPU;
		m_12CBView[Idx].HandleGPU = HandleGPU;
		m_12CBView[Idx].Desc.BufferLocation = Address;
		m_12CBView[Idx].Desc.SizeInBytes = (sizeof(ConstantBuffer) + 0xff) & ~0xff;

		// 定数バッファビュー生成
		Device->CreateConstantBufferView(&m_12CBView[Idx].Desc, HandleCPU);

		// 定数データ転送開始
		hr = m_12ConstantBuffer[Idx]->Map(0, nullptr, reinterpret_cast<void**>(&m_12CBView[Idx].pBuffer));
		if (FAILED(hr))
		{
			return false;
		}

		// 変換行列設定
		XMStoreFloat4x4(&m_12CBView[Idx].pBuffer->world, DirectX::XMMatrixIdentity());
		m_12CBView[Idx].pBuffer->view = Camera::GetInstance().GetViewMatrix();
		m_12CBView[Idx].pBuffer->projection = Camera::GetInstance().GetProjMatrix();
	}

	auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	// ルートパラメータ設定
	D3D12_ROOT_PARAMETER RootParam[2] = {};
	RootParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	RootParam[0].Descriptor.ShaderRegister = 0;
	RootParam[0].Descriptor.RegisterSpace = 0;
	RootParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	// 
	D3D12_DESCRIPTOR_RANGE SRVDescRange = {};
	SRVDescRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	SRVDescRange.NumDescriptors = 1;
	SRVDescRange.BaseShaderRegister = 0;
	SRVDescRange.RegisterSpace = 0;
	SRVDescRange.OffsetInDescriptorsFromTableStart = 0;

	RootParam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	RootParam[1].DescriptorTable.NumDescriptorRanges = 1;
	RootParam[1].DescriptorTable.pDescriptorRanges = &SRVDescRange;
	RootParam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	// スタティックサンプラー設定
	D3D12_STATIC_SAMPLER_DESC Sampler = {};
	Sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	Sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	Sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	Sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	Sampler.MaxAnisotropy = 1;
	Sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	Sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	Sampler.MinLOD = -D3D12_FLOAT32_MAX;
	Sampler.MaxLOD = +D3D12_FLOAT32_MAX;

	// ルートシグネチャ設定
	D3D12_ROOT_SIGNATURE_DESC RootDesc = {};
	RootDesc.NumParameters = 2;
	RootDesc.NumStaticSamplers = 1;
	RootDesc.pParameters = RootParam;
	RootDesc.pStaticSamplers = &Sampler;
	RootDesc.Flags = flag;

	ComPtr<ID3DBlob> pBlob;
	ComPtr<ID3DBlob> pErrorBlob;

	// シリアライズ
	hr = D3D12SerializeRootSignature(&RootDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		pBlob.GetAddressOf(), pErrorBlob.ReleaseAndGetAddressOf());
	if (FAILED(hr)) return false;

	// ルートシグニチャを生成
	hr = Device->CreateRootSignature(0, pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(), IID_PPV_ARGS(m_12RootSignature.ReleaseAndGetAddressOf()));
	if (FAILED(hr)) return false;

	D3D12_INPUT_ELEMENT_DESC VertexLayout[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT	  ,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"COLOR"  , 0, DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"NORMAL"  , 0,DXGI_FORMAT_R32G32B32_FLOAT	  ,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT	  ,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
	};

	// レンダーターゲットのブレンド設定.
	D3D12_RENDER_TARGET_BLEND_DESC RTBDesc = {
		FALSE, FALSE,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_LOGIC_OP_NOOP,
		D3D12_COLOR_WRITE_ENABLE_ALL
	};

	// ブレンドステートの設定.
	D3D12_BLEND_DESC BSDesc;
	BSDesc.AlphaToCoverageEnable = true;
	BSDesc.IndependentBlendEnable = FALSE;
	for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
	{
		BSDesc.RenderTarget[i] = RTBDesc;
	}

	// 深度ステンシルステート設定
	D3D12_DEPTH_STENCIL_DESC DepStencilDesc = {};
	DepStencilDesc.DepthEnable = false;
	DepStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	DepStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	DepStencilDesc.StencilEnable = false;

	ComPtr<ID3DBlob> VSBlob;
	ComPtr<ID3DBlob> PSBlob;

	// 頂点シェーダー読み込み
	if (!CompileShader("Shader/SimpleVS.hlsl", "main", "vs_5_0", VSBlob.ReleaseAndGetAddressOf()))
	{
		return false;
	}

	// ピクセルシェーダー読み込み
	if (!CompileShader("Shader/TexturePS.hlsl", "main", "ps_5_0", PSBlob.ReleaseAndGetAddressOf()))
	{
		return false;
	}

	// パイプラインステート設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC PipeStateDesc = {};
	PipeStateDesc.InputLayout = { VertexLayout, _countof(VertexLayout) };
	PipeStateDesc.pRootSignature = m_12RootSignature.Get();
	PipeStateDesc.VS.BytecodeLength = VSBlob->GetBufferSize();
	PipeStateDesc.VS.pShaderBytecode = VSBlob->GetBufferPointer();
	PipeStateDesc.PS.BytecodeLength = PSBlob->GetBufferSize();
	PipeStateDesc.PS.pShaderBytecode = PSBlob->GetBufferPointer();

	// ラスタライザーステート設定
	D3D12_RASTERIZER_DESC RasDesc;
	RasDesc.FillMode = D3D12_FILL_MODE_SOLID;
	RasDesc.CullMode = D3D12_CULL_MODE_BACK;
	RasDesc.FrontCounterClockwise = FALSE;
	RasDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	RasDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	RasDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	RasDesc.DepthClipEnable = FALSE;
	RasDesc.MultisampleEnable = FALSE;
	RasDesc.AntialiasedLineEnable = FALSE;
	RasDesc.ForcedSampleCount = 0;
	RasDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	PipeStateDesc.RasterizerState = RasDesc;
	PipeStateDesc.BlendState = BSDesc;
	PipeStateDesc.DepthStencilState = DepStencilDesc;
	PipeStateDesc.SampleMask = UINT_MAX;
	PipeStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipeStateDesc.NumRenderTargets = 1;
	PipeStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	PipeStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	PipeStateDesc.SampleDesc.Count = 1;
	PipeStateDesc.SampleDesc.Quality = 0;

	// パイプラインステート生成
	hr = Device->CreateGraphicsPipelineState(&PipeStateDesc, IID_PPV_ARGS(m_12PSO.ReleaseAndGetAddressOf()));
	if (FAILED(hr)) return false;

	// テクスチャ生成
	CreateTexture(Device);

	return true;
}

bool Object::CreateTexture(ID3D12Device* Device)
{
	struct TexRGBA
	{
		unsigned char R, G, B, A;
	};
	std::vector<TexRGBA> TextureData(256 * 256);

	for (auto& rgba : TextureData)
	{
		rgba.R = rand() % 256;
		rgba.G = rand() % 256;
		rgba.B = rand() % 256;
		rgba.A = 255;
	}

	// テクスチャ用バッファ設定
	D3D12_HEAP_PROPERTIES TexHeapProp = {};
	TexHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	TexHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	TexHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	TexHeapProp.CreationNodeMask = 0;
	TexHeapProp.VisibleNodeMask = 0;

	// テクスチャ設定
	D3D12_RESOURCE_DESC TexResDesc = {};
	TexResDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	TexResDesc.Width = 256;
	TexResDesc.Height = 256;
	TexResDesc.DepthOrArraySize = 1;
	TexResDesc.SampleDesc.Count = 1;
	TexResDesc.SampleDesc.Quality = 0;
	TexResDesc.MipLevels = 1;
	TexResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	TexResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	TexResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// テクスチャリソース生成
	HRESULT hr = Device->CreateCommittedResource(&TexHeapProp, D3D12_HEAP_FLAG_NONE, &TexResDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr, IID_PPV_ARGS(m_12Texture.pResouce.ReleaseAndGetAddressOf()));
	if (FAILED(hr)) return false;

	// テクスチャ書き込み
	hr = m_12Texture.pResouce->WriteToSubresource(0, nullptr, TextureData.data(), sizeof(TexRGBA) * 256, sizeof(TexRGBA) * (UINT)TextureData.size());
	if (FAILED(hr)) return false;

	// インクリメント用サイズ取得
	auto IncrementSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	auto HandleCPU = m_12BasicDescHeap->GetCPUDescriptorHandleForHeapStart();
	auto HandleGPU = m_12BasicDescHeap->GetGPUDescriptorHandleForHeapStart();

	HandleCPU.ptr += IncrementSize;
	HandleGPU.ptr += IncrementSize;

	m_12Texture.HandleCPU = HandleCPU;
	m_12Texture.HandleGPU = HandleGPU;

	auto TexDesc = m_12Texture.pResouce->GetDesc();

	// シェーダーリソースビュー用設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = TexResDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = TexDesc.MipLevels;
	srvDesc.Texture2D.PlaneSlice = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	// シェーダーリソースビュー生成
	Device->CreateShaderResourceView(m_12Texture.pResouce.Get(), &srvDesc, HandleCPU);

	return true;
}

bool Object::CreateTexture(ID3D11Device* Device, ID3D11DeviceContext* Context)
{
	struct TexRGBA
	{
		unsigned char R, G, B, A;
	};
	std::vector<TexRGBA> TextureData(256 * 256*4);

	for (auto& rgba : TextureData)
	{
		rgba.R = rand() % 256;
		rgba.G = rand() % 256;
		rgba.B = rand() % 256;
		rgba.A = 255;
	}

	// テクスチャの生成
	ComPtr<ID3D11Texture2D> CreateTexture;

	// テクスチャ設定
	D3D11_TEXTURE2D_DESC TexDesc;
	TexDesc.Width = 256;
	TexDesc.Height = 256;
	TexDesc.MipLevels = 1;
	TexDesc.ArraySize = 1;
	TexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	TexDesc.SampleDesc.Count = 1;
	TexDesc.SampleDesc.Quality = 0;
	TexDesc.Usage = D3D11_USAGE_DYNAMIC;
	TexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TexDesc.MiscFlags = 0;

	// テクスチャ生成
	if (FAILED(Device->CreateTexture2D(&TexDesc, nullptr, &CreateTexture)))
	{
		return false;
	}

	// テクスチャの書き換え
	D3D11_MAPPED_SUBRESOURCE MapSubResouce;;
	Context->Map(CreateTexture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MapSubResouce);

	// データ書き込み
	memcpy(MapSubResouce.pData, TextureData.data(), TextureData.size());
	Context->Unmap(CreateTexture.Get(), 0);

	// シェーダーリソースビュー設定
	D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
	srv.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv.Texture2D.MipLevels = 1;

	// シェーダーリソースビュー生成
	if (FAILED(Device->CreateShaderResourceView(CreateTexture.Get(), &srv, &m_11NoiseTextureResouce)))
	{
		return false;
	}

	//サンプラステート設定
	D3D11_SAMPLER_DESC sd = {};
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	// サンプラーステート生成
	if (FAILED(Device->CreateSamplerState(&sd, &m_11SamplerState)))
	{
		return false;
	}

	return true;
}

// オブジェクト描画
void Object::Draw(ID3D11DeviceContext* CmdList)
{
	UINT strides = sizeof(Vertex);
	UINT offsets = 0;

	CmdList->IASetInputLayout(m_11InputLayOut.Get());
	CmdList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	CmdList->IASetVertexBuffers(0, 1, m_11VertexBuffer.GetAddressOf(), &strides, &offsets);
	CmdList->IASetIndexBuffer(m_11IndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	CmdList->VSSetConstantBuffers(0, 1, m_11ConstantBuffer.GetAddressOf());

	CmdList->VSSetShader(m_11VertexShader.Get(), NULL, 0);
	CmdList->PSSetShader(m_11PixelShader.Get(), NULL, 0);

	CmdList->PSSetShaderResources(0, 1, m_11NoiseTextureResouce.GetAddressOf());
	CmdList->PSSetSamplers(0, 1, m_11SamplerState.GetAddressOf());

	CmdList->DrawIndexed(m_IndexNum, 0, 0);
}

void Object::Draw(ID3D12GraphicsCommandList* CmdList, int FrameIndex)
{
	CmdList->SetGraphicsRootSignature(m_12RootSignature.Get());
	CmdList->SetDescriptorHeaps(1, m_12BasicDescHeap.GetAddressOf());
	CmdList->SetGraphicsRootConstantBufferView(0, m_12CBView[FrameIndex].Desc.BufferLocation);
	CmdList->SetGraphicsRootDescriptorTable(1, m_12Texture.HandleGPU);
	CmdList->SetPipelineState(m_12PSO.Get());

	CmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CmdList->IASetVertexBuffers(0, 1, &m_12VBView);
	CmdList->IASetIndexBuffer(&m_12IBView);

	CmdList->DrawIndexedInstanced(m_IndexNum, 1, 0, 0, 0);
}

// オブジェクト更新
void Object::Update(int FrameIndex)
{
	m_RotateAngle += 0.025f;

	// 拡大縮小*回転*移動
	auto Matrix = XMMatrixRotationY(m_RotateAngle)*XMMatrixTranslation(m_Pos.x,m_Pos.y,m_Pos.z);
	XMStoreFloat4x4(&m_12CBView[FrameIndex].pBuffer->world, Matrix);
	m_12CBView[FrameIndex].pBuffer->view = Camera::GetInstance().GetViewMatrix();
	m_12CBView[FrameIndex].pBuffer->projection = Camera::GetInstance().GetProjMatrix();
}

void Object::Update(ID3D11DeviceContext* Context)
{
	m_RotateAngle += 0.025f;
	auto Matrix = XMMatrixRotationY(m_RotateAngle);

	// 行列変換
	ConstantBuffer cb;
	XMStoreFloat4x4(&cb.world, Matrix);
	cb.view = Camera::GetInstance().GetViewMatrix();
	cb.projection = Camera::GetInstance().GetProjMatrix();

	// ライト
	XMVECTOR Light = XMVector3Normalize(XMVectorSet(0.0f, 0.5f, -1.0f, 0.0f));
	XMStoreFloat4(&cb.LightDir, Light);

	Context->UpdateSubresource(m_11ConstantBuffer.Get(), 0, NULL, &cb, 0, 0);

}
