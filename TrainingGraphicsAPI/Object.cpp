#include "Object.h"

bool Object::QuadInit(ID3D12Device* Device)
{
	// ���_���X�g
	Vertex VertexList[]
	{
		{{-0.5f,-0.5f, 0.0f},{ 1.0f, 0.0f, 0.0f, 1.0f},{ 0.0f, 0.0f,-1.0f},{0.0f,0.0f}},
		{{-0.5f, 0.5f, 0.0f},{ 1.0f, 0.0f, 0.0f, 1.0f},{ 0.0f, 0.0f,-1.0f},{1.0f,0.0f}},
		{{ 0.5f,-0.5f, 0.0f},{ 1.0f, 0.0f, 0.0f, 1.0f},{ 0.0f, 0.0f,-1.0f},{0.0f,1.0f}},
		{{ 0.5f, 0.5f, 0.0f},{ 1.0f, 0.0f, 0.0f, 1.0f},{ 0.0f, 0.0f,-1.0f},{1.0f,1.0f}},
	};

	// �C���f�b�N�X���X�g
	uint32_t IndexList[] =
	{
		 0,  1,  2,   3,  2,  1,
	};

	// ���_�o�b�t�@�ݒ�
	D3D12_HEAP_PROPERTIES VBProp = {};
	VBProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	VBProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	VBProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	VBProp.CreationNodeMask = 1;
	VBProp.VisibleNodeMask = 1;

	// ���\�[�X�ݒ�
	D3D12_RESOURCE_DESC VBResDesc = {};
	VBResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	VBResDesc.Alignment = 0;
	VBResDesc.Width = sizeof(VertexList);
	VBResDesc.Height = 1;
	VBResDesc.DepthOrArraySize = 1;
	VBResDesc.MipLevels = 1;
	VBResDesc.Format = DXGI_FORMAT_UNKNOWN;
	VBResDesc.SampleDesc.Count = 1;
	VBResDesc.SampleDesc.Quality = 0;
	VBResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	VBResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// ���_�o�b�t�@����
	auto hr = Device->CreateCommittedResource(&VBProp, D3D12_HEAP_FLAG_NONE,
		&VBResDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(m_VertexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr)) return false;

	// ���_�f�[�^�]���J�n
	void* SendVertexData = nullptr;
	hr = m_VertexBuffer->Map(0, nullptr, &SendVertexData);
	if (FAILED(hr)) hr;

	// �]��
	memcpy(SendVertexData, VertexList, sizeof(VertexList));

	// ���_�f�[�^�]���I��
	m_VertexBuffer->Unmap(0, nullptr);

	// ���_�o�b�t�@�[�r���[�ݒ�
	m_VBView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VBView.SizeInBytes = static_cast<UINT>(sizeof(VertexList));
	m_VBView.StrideInBytes = static_cast<UINT>(sizeof(Vertex));

	// �`��p�C���f�b�N�X���擾
	m_IndexNum = ARRAYSIZE(IndexList);

	// �C���f�b�N�X�o�b�t�@�ݒ�
	D3D12_HEAP_PROPERTIES IBProp = {};
	IBProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	IBProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	IBProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	IBProp.CreationNodeMask = 1;
	IBProp.VisibleNodeMask = 1;

	// ���\�[�X�̐ݒ�
	D3D12_RESOURCE_DESC IBResDesc = {};
	IBResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	IBResDesc.Alignment = 0;
	IBResDesc.Width = sizeof(IndexList);
	IBResDesc.Height = 1;
	IBResDesc.DepthOrArraySize = 1;
	IBResDesc.MipLevels = 1;
	IBResDesc.Format = DXGI_FORMAT_UNKNOWN;
	IBResDesc.SampleDesc.Count = 1;
	IBResDesc.SampleDesc.Quality = 0;
	IBResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	IBResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// �C���f�b�N�X�o�b�t�@����
	hr = Device->CreateCommittedResource(&IBProp, D3D12_HEAP_FLAG_NONE,
		&IBResDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(m_IndexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr)) return false;

	// �C���f�b�N�X�f�[�^�]���J�n
	void* SendIndexData = nullptr;
	hr = m_IndexBuffer->Map(0, nullptr, &SendIndexData);
	if (FAILED(hr)) return false;

	// �]��
	memcpy(SendIndexData, IndexList, sizeof(IndexList));

	// �C���f�b�N�X�f�[�^�]���I��
	m_IndexBuffer->Unmap(0, nullptr);

	// �C���f�b�N�X�o�b�t�@�r���[�ݒ�
	m_IBView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
	m_IBView.Format = DXGI_FORMAT_R32_UINT;
	m_IBView.SizeInBytes = sizeof(IndexList);

	// �萔�o�b�t�@�p�f�B�X�N���v�^�q�[�v�ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC BasicDesc = {};
	BasicDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	BasicDesc.NumDescriptors = 2;
	BasicDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	BasicDesc.NodeMask = 0;

	// CBV/SRV/UAV�p�f�B�X�N���v�^�q�[�v����
	hr = Device->CreateDescriptorHeap(&BasicDesc, IID_PPV_ARGS(m_BasicDescHeap.GetAddressOf()));
	if (FAILED(hr)) return false;

	// �萔�o�b�t�@�ݒ�
	D3D12_HEAP_PROPERTIES CBProp = {};
	CBProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	CBProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	CBProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	CBProp.CreationNodeMask = 1;
	CBProp.VisibleNodeMask = 1;

	// ���\�[�X�̐ݒ�.
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

	// �C���N�������g�p�T�C�Y�擾
	auto incrementSize = Device
		->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	for (auto Idx = 0; Idx < 2; ++Idx)
	{
		hr = Device->CreateCommittedResource(&CBProp, D3D12_HEAP_FLAG_NONE,
			&CBResDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(m_ConstantBuffer[Idx].ReleaseAndGetAddressOf()));
		if (FAILED(hr)) return false;

		auto Address = m_ConstantBuffer[Idx]->GetGPUVirtualAddress();
		auto HandleCPU = m_BasicDescHeap->GetCPUDescriptorHandleForHeapStart();
		auto HandleGPU = m_BasicDescHeap->GetGPUDescriptorHandleForHeapStart();

		HandleCPU.ptr += incrementSize * Idx;
		HandleGPU.ptr += incrementSize * Idx;

		// �萔�o�b�t�@�r���[�ݒ�
		m_CBView[Idx].HandleCPU = HandleCPU;
		m_CBView[Idx].HandleGPU = HandleGPU;
		m_CBView[Idx].Desc.BufferLocation = Address;
		m_CBView[Idx].Desc.SizeInBytes = (sizeof(ConstantBuffer) + 0xff) & ~0xff;

		// �萔�o�b�t�@�r���[����
		Device->CreateConstantBufferView(&m_CBView[Idx].Desc, HandleCPU);

		// �萔�f�[�^�]���J�n
		hr = m_ConstantBuffer[Idx]->Map(0, nullptr, reinterpret_cast<void**>(&m_CBView[Idx].pBuffer));
		if (FAILED(hr))
		{
			return false;
		}

		// �ϊ��s��ݒ�
		XMStoreFloat4x4(&m_CBView[Idx].pBuffer->world, DirectX::XMMatrixIdentity());
		m_CBView[Idx].pBuffer->view = Camera::GetInstance().GetViewMatrix();
		m_CBView[Idx].pBuffer->projection = Camera::GetInstance().GetProjMatrix();
	}

	auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	// ���[�g�p�����[�^�ݒ�
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

	// �X�^�e�B�b�N�T���v���[�ݒ�
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

	// ���[�g�V�O�l�`���ݒ�
	D3D12_ROOT_SIGNATURE_DESC RootDesc = {};
	RootDesc.NumParameters = 2;
	RootDesc.NumStaticSamplers = 1;
	RootDesc.pParameters = RootParam;
	RootDesc.pStaticSamplers = &Sampler;
	RootDesc.Flags = flag;

	ComPtr<ID3DBlob> pBlob;
	ComPtr<ID3DBlob> pErrorBlob;

	// �V���A���C�Y
	hr = D3D12SerializeRootSignature(&RootDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		pBlob.GetAddressOf(), pErrorBlob.ReleaseAndGetAddressOf());
	if (FAILED(hr)) return false;

	// ���[�g�V�O�j�`���𐶐�
	hr = Device->CreateRootSignature(0, pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(), IID_PPV_ARGS(m_RootSignature.ReleaseAndGetAddressOf()));
	if (FAILED(hr)) return false;

	D3D12_INPUT_ELEMENT_DESC VertexLayout[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT	  ,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{ "COLOR"  , 0, DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"NORMAL"  , 0,DXGI_FORMAT_R32G32B32_FLOAT	  ,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT	  ,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
	};

	// �����_�[�^�[�Q�b�g�̃u�����h�ݒ�.
	D3D12_RENDER_TARGET_BLEND_DESC RTBDesc = {
		FALSE, FALSE,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_LOGIC_OP_NOOP,
		D3D12_COLOR_WRITE_ENABLE_ALL
	};

	// �u�����h�X�e�[�g�̐ݒ�.
	D3D12_BLEND_DESC BSDesc;
	BSDesc.AlphaToCoverageEnable = true;
	BSDesc.IndependentBlendEnable = FALSE;
	for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
	{
		BSDesc.RenderTarget[i] = RTBDesc;
	}

	// �[�x�X�e���V���X�e�[�g�ݒ�
	D3D12_DEPTH_STENCIL_DESC DepStencilDesc = {};
	DepStencilDesc.DepthEnable = false;
	DepStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	DepStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	DepStencilDesc.StencilEnable = false;

	ComPtr<ID3DBlob> VSBlob;
	ComPtr<ID3DBlob> PSBlob;

	// ���_�V�F�[�_�[�ǂݍ���
	if (!CompileShader("Shader/SimpleVS.hlsl", "main", "vs_5_0", VSBlob.ReleaseAndGetAddressOf()))
	{
		return false;
	}

	// �s�N�Z���V�F�[�_�[�ǂݍ���
	if (!CompileShader("Shader/TexturePS.hlsl", "main", "ps_5_0", PSBlob.ReleaseAndGetAddressOf()))
	{
		return false;
	}

	// �p�C�v���C���X�e�[�g�ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC PipeStateDesc = {};
	PipeStateDesc.InputLayout = { VertexLayout, _countof(VertexLayout) };
	PipeStateDesc.pRootSignature = m_RootSignature.Get();
	PipeStateDesc.VS.BytecodeLength = VSBlob->GetBufferSize();
	PipeStateDesc.VS.pShaderBytecode = VSBlob->GetBufferPointer();
	PipeStateDesc.PS.BytecodeLength = PSBlob->GetBufferSize();
	PipeStateDesc.PS.pShaderBytecode = PSBlob->GetBufferPointer();

	// ���X�^���C�U�[�X�e�[�g�ݒ�
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

	// �p�C�v���C���X�e�[�g����
	hr = Device->CreateGraphicsPipelineState(&PipeStateDesc, IID_PPV_ARGS(m_PSO.ReleaseAndGetAddressOf()));
	if (FAILED(hr)) return false;

	// �e�N�X�`������
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

	//for8

	// �e�N�X�`���p�o�b�t�@�ݒ�
	D3D12_HEAP_PROPERTIES TexHeapProp = {};
	TexHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	TexHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	TexHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	TexHeapProp.CreationNodeMask = 0;
	TexHeapProp.VisibleNodeMask = 0;

	// �e�N�X�`���ݒ�
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

	// �e�N�X�`�����\�[�X����
	HRESULT hr = Device->CreateCommittedResource(&TexHeapProp, D3D12_HEAP_FLAG_NONE, &TexResDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr, IID_PPV_ARGS(m_Texture.pResouce.ReleaseAndGetAddressOf()));
	if (FAILED(hr)) return false;

	// �e�N�X�`����������
	hr = m_Texture.pResouce->WriteToSubresource(0, nullptr, TextureData.data(), sizeof(TexRGBA) * 256, sizeof(TexRGBA) * (UINT)TextureData.size());
	if (FAILED(hr)) return false;

	// �C���N�������g�p�T�C�Y�擾
	auto IncrementSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	auto HandleCPU = m_BasicDescHeap->GetCPUDescriptorHandleForHeapStart();
	auto HandleGPU = m_BasicDescHeap->GetGPUDescriptorHandleForHeapStart();

	HandleCPU.ptr += IncrementSize;
	HandleGPU.ptr += IncrementSize;

	m_Texture.HandleCPU = HandleCPU;
	m_Texture.HandleGPU = HandleGPU;

	auto TexDesc = m_Texture.pResouce->GetDesc();

	// �V�F�[�_�[���\�[�X�r���[�p�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = TexResDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = TexDesc.MipLevels;
	srvDesc.Texture2D.PlaneSlice = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	// �V�F�[�_�[���\�[�X�r���[����
	Device->CreateShaderResourceView(m_Texture.pResouce.Get(), &srvDesc, HandleCPU);

	return true;
}

// �I�u�W�F�N�g�`��
void Object::Draw(ID3D12GraphicsCommandList* CmdList, int FrameIndex)
{
	CmdList->SetGraphicsRootSignature(m_RootSignature.Get());
	CmdList->SetDescriptorHeaps(1, m_BasicDescHeap.GetAddressOf());
	CmdList->SetGraphicsRootConstantBufferView(0, m_CBView[FrameIndex].Desc.BufferLocation);
	CmdList->SetGraphicsRootDescriptorTable(1, m_Texture.HandleGPU);
	CmdList->SetPipelineState(m_PSO.Get());

	CmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CmdList->IASetVertexBuffers(0, 1, &m_VBView);
	CmdList->IASetIndexBuffer(&m_IBView);

	CmdList->DrawIndexedInstanced(m_IndexNum, 1, 0, 0, 0);
}

// �I�u�W�F�N�g�X�V
void Object::Update(int FrameIndex)
{
	m_RotateAngle += 0.025f;
	auto Matrix = XMMatrixRotationY(m_RotateAngle);
	XMStoreFloat4x4(&m_CBView[FrameIndex].pBuffer->world, Matrix);
	m_CBView[FrameIndex].pBuffer->view = Camera::GetInstance().GetViewMatrix();
	m_CBView[FrameIndex].pBuffer->projection = Camera::GetInstance().GetProjMatrix();
}

bool Object::CubeInit(ID3D12Device* Device)
{
	// ���_���X�g
	Vertex VertexList[]
	{
		// �O��
		{{-0.5f,-0.5f, 0.5f},{ 1.0f, 0.0f, 0.0f, 1.0f},{ 0.0f, 0.0f,-1.0f},{0.0f,0.0f}},
		{{-0.5f, 0.5f, 0.5f},{ 1.0f, 0.0f, 0.0f, 1.0f},{ 0.0f, 0.0f,-1.0f},{1.0f,0.0f}},
		{{ 0.5f,-0.5f, 0.5f},{ 1.0f, 0.0f, 0.0f, 1.0f},{ 0.0f, 0.0f,-1.0f},{0.0f,1.0f}},
		{{ 0.5f, 0.5f, 0.5f},{ 1.0f, 0.0f, 0.0f, 1.0f},{ 0.0f, 0.0f,-1.0f},{1.0f,1.0f}},

		// ���
		{{-0.5f,-0.5f,-0.5f},{ 0.0f, 1.0f, 1.0f, 1.0f},{ 0.0f, 0.0f, 1.0f},{0.0f,0.0f}},
		{{-0.5f, 0.5f,-0.5f},{ 0.0f, 1.0f, 1.0f, 1.0f},{ 0.0f, 0.0f, 1.0f},{1.0f,0.0f}},
		{{ 0.5f,-0.5f,-0.5f},{ 0.0f, 1.0f, 1.0f, 1.0f},{ 0.0f, 0.0f, 1.0f},{0.0f,1.0f}},
		{{ 0.5f, 0.5f,-0.5f},{ 0.0f, 1.0f, 1.0f, 1.0f},{ 0.0f, 0.0f, 1.0f},{1.0f,1.0f}},

		// �E��
		{{ 0.5f, 0.5f, 0.5f},{ 1.0f, 1.0f, 0.0f, 1.0f},{-1.0f, 0.0f, 0.0f},{0.0f,0.0f}},
		{{ 0.5f, 0.5f,-0.5f},{ 1.0f, 1.0f, 0.0f, 1.0f},{-1.0f, 0.0f, 0.0f},{1.0f,0.0f}},
		{{ 0.5f,-0.5f, 0.5f},{ 1.0f, 1.0f, 0.0f, 1.0f},{-1.0f, 0.0f, 0.0f},{0.0f,1.0f}},
		{{ 0.5f,-0.5f,-0.5f},{ 1.0f, 1.0f, 0.0f, 1.0f},{-1.0f, 0.0f, 0.0f},{1.0f,1.0f}},

		// ����
		{{-0.5f, 0.5f, 0.5f},{ 0.0f, 0.0f, 1.0f, 1.0f},{ 1.0f, 0.0f, 0.0f},{0.0f,0.0f}},
		{{-0.5f,-0.5f, 0.5f},{ 0.0f, 0.0f, 1.0f, 1.0f},{ 1.0f, 0.0f, 0.0f},{1.0f,0.0f}},
		{{-0.5f, 0.5f,-0.5f},{ 0.0f, 0.0f, 1.0f, 1.0f},{ 1.0f, 0.0f, 0.0f},{0.0f,1.0f}},
		{{-0.5f,-0.5f,-0.5f},{ 0.0f, 0.0f, 1.0f, 1.0f},{ 1.0f, 0.0f, 0.0f},{1.0f,1.0f}},


		// ���
		{{-0.5f, 0.5f, 0.5f},{ 0.0f, 1.0f, 0.0f, 1.0f},{ 0.0f,-1.0f, 0.0f},{0.0f,0.0f}},
		{{-0.5f, 0.5f,-0.5f},{ 0.0f, 1.0f, 0.0f, 1.0f},{ 0.0f,-1.0f, 0.0f},{1.0f,0.0f}},
		{{ 0.5f, 0.5f, 0.5f},{ 0.0f, 1.0f, 0.0f, 1.0f},{ 0.0f,-1.0f, 0.0f},{0.0f,1.0f}},
		{{ 0.5f, 0.5f,-0.5f},{ 0.0f, 1.0f, 0.0f, 1.0f},{ 0.0f,-1.0f, 0.0f},{1.0f,1.0f}},

		// ����
		{{-0.5f,-0.5f, 0.5f},{ 1.0f, 0.0f, 1.0f, 1.0f},{ 0.0f, 1.0f, 0.0f},{0.0f,0.0f}},
		{{ 0.5f,-0.5f, 0.5f},{ 1.0f, 0.0f, 1.0f, 1.0f},{ 0.0f, 1.0f, 0.0f},{1.0f,0.0f}},
		{{-0.5f,-0.5f,-0.5f},{ 1.0f, 0.0f, 1.0f, 1.0f},{ 0.0f, 1.0f, 0.0f},{0.0f,1.0f}},
		{{ 0.5f,-0.5f,-0.5f},{ 1.0f, 0.0f, 1.0f, 1.0f},{ 0.0f, 1.0f, 0.0f},{1.0f,1.0f}},

	};

	// �C���f�b�N�X���X�g
	uint32_t IndexList[] =
	{
		 0,   1,  2,     3,  2,  1,	 // �O��
		 6,   5,  4,     5,  6,  7,  // ���
		 8,   9, 10,    11, 10,  9,	 // �E��
		12,  13, 14,    15, 14, 13,	 // ����
		16,  17, 18,    19, 18, 17,	 // ���
		20,  21, 22,    23, 22, 21,  // ����
	};


	// ���_�o�b�t�@�ݒ�
	D3D12_HEAP_PROPERTIES VBProp = {};
	VBProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	VBProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	VBProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	VBProp.CreationNodeMask = 1;
	VBProp.VisibleNodeMask = 1;

	// ���\�[�X�ݒ�
	D3D12_RESOURCE_DESC VBResDesc = {};
	VBResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	VBResDesc.Alignment = 0;
	VBResDesc.Width = sizeof(VertexList);
	VBResDesc.Height = 1;
	VBResDesc.DepthOrArraySize = 1;
	VBResDesc.MipLevels = 1;
	VBResDesc.Format = DXGI_FORMAT_UNKNOWN;
	VBResDesc.SampleDesc.Count = 1;
	VBResDesc.SampleDesc.Quality = 0;
	VBResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	VBResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// ���_�o�b�t�@����
	auto hr = Device->CreateCommittedResource(&VBProp, D3D12_HEAP_FLAG_NONE,
		&VBResDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(m_VertexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr)) return false;

	// ���_�f�[�^�]���J�n
	void* SendVertexData = nullptr;
	hr = m_VertexBuffer->Map(0, nullptr, &SendVertexData);
	if (FAILED(hr)) hr;

	// �]��
	memcpy(SendVertexData, VertexList, sizeof(VertexList));

	// ���_�f�[�^�]���I��
	m_VertexBuffer->Unmap(0, nullptr);

	// ���_�o�b�t�@�[�r���[�ݒ�
	m_VBView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VBView.SizeInBytes = static_cast<UINT>(sizeof(VertexList));
	m_VBView.StrideInBytes = static_cast<UINT>(sizeof(Vertex));

	// �`��p�C���f�b�N�X���擾
	m_IndexNum = ARRAYSIZE(IndexList);

	// �C���f�b�N�X�o�b�t�@�ݒ�
	D3D12_HEAP_PROPERTIES IBProp = {};
	IBProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	IBProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	IBProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	IBProp.CreationNodeMask = 1;
	IBProp.VisibleNodeMask = 1;

	// ���\�[�X�̐ݒ�
	D3D12_RESOURCE_DESC IBResDesc = {};
	IBResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	IBResDesc.Alignment = 0;
	IBResDesc.Width = sizeof(IndexList);
	IBResDesc.Height = 1;
	IBResDesc.DepthOrArraySize = 1;
	IBResDesc.MipLevels = 1;
	IBResDesc.Format = DXGI_FORMAT_UNKNOWN;
	IBResDesc.SampleDesc.Count = 1;
	IBResDesc.SampleDesc.Quality = 0;
	IBResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	IBResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// �C���f�b�N�X�o�b�t�@����
	hr = Device->CreateCommittedResource(&IBProp, D3D12_HEAP_FLAG_NONE,
		&IBResDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(m_IndexBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr)) return false;

	// �C���f�b�N�X�f�[�^�]���J�n
	void* SendIndexData = nullptr;
	hr = m_IndexBuffer->Map(0, nullptr, &SendIndexData);
	if (FAILED(hr)) return false;

	// �]��
	memcpy(SendIndexData, IndexList, sizeof(IndexList));

	// �C���f�b�N�X�f�[�^�]���I��
	m_IndexBuffer->Unmap(0, nullptr);

	// �C���f�b�N�X�o�b�t�@�r���[�ݒ�
	m_IBView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
	m_IBView.Format = DXGI_FORMAT_R32_UINT;
	m_IBView.SizeInBytes = sizeof(IndexList);

	// �萔�o�b�t�@�p�f�B�X�N���v�^�q�[�v�ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC BasicDesc = {};
	BasicDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	BasicDesc.NumDescriptors = 2;
	BasicDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	BasicDesc.NodeMask = 0;

	// CBV/SRV/UAV�p�f�B�X�N���v�^�q�[�v����
	hr = Device->CreateDescriptorHeap(&BasicDesc, IID_PPV_ARGS(m_BasicDescHeap.GetAddressOf()));
	if (FAILED(hr)) return false;

	// �萔�o�b�t�@�ݒ�
	D3D12_HEAP_PROPERTIES CBProp = {};
	CBProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	CBProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	CBProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	CBProp.CreationNodeMask = 1;
	CBProp.VisibleNodeMask = 1;

	// ���\�[�X�̐ݒ�.
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

	// �C���N�������g�p�T�C�Y�擾
	auto incrementSize = Device
		->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	for (auto Idx = 0; Idx < 2; ++Idx)
	{
		hr = Device->CreateCommittedResource(&CBProp, D3D12_HEAP_FLAG_NONE,
			&CBResDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(m_ConstantBuffer[Idx].ReleaseAndGetAddressOf()));
		if (FAILED(hr)) return false;

		auto Address = m_ConstantBuffer[Idx]->GetGPUVirtualAddress();
		auto HandleCPU = m_BasicDescHeap->GetCPUDescriptorHandleForHeapStart();
		auto HandleGPU = m_BasicDescHeap->GetGPUDescriptorHandleForHeapStart();

		HandleCPU.ptr += incrementSize * Idx;
		HandleGPU.ptr += incrementSize * Idx;

		// �萔�o�b�t�@�r���[�ݒ�
		m_CBView[Idx].HandleCPU = HandleCPU;
		m_CBView[Idx].HandleGPU = HandleGPU;
		m_CBView[Idx].Desc.BufferLocation = Address;
		m_CBView[Idx].Desc.SizeInBytes = (sizeof(ConstantBuffer) + 0xff) & ~0xff;

		// �萔�o�b�t�@�r���[����
		Device->CreateConstantBufferView(&m_CBView[Idx].Desc, HandleCPU);

		// �萔�f�[�^�]���J�n
		hr = m_ConstantBuffer[Idx]->Map(0, nullptr, reinterpret_cast<void**>(&m_CBView[Idx].pBuffer));
		if (FAILED(hr))
		{
			return false;
		}

		// �ϊ��s��ݒ�
		XMStoreFloat4x4(&m_CBView[Idx].pBuffer->world, DirectX::XMMatrixIdentity());
		m_CBView[Idx].pBuffer->view = Camera::GetInstance().GetViewMatrix();
		m_CBView[Idx].pBuffer->projection = Camera::GetInstance().GetProjMatrix();
	}

	auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	// ���[�g�p�����[�^�ݒ�
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

	// �X�^�e�B�b�N�T���v���[�ݒ�
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

	// ���[�g�V�O�l�`���ݒ�
	D3D12_ROOT_SIGNATURE_DESC RootDesc = {};
	RootDesc.NumParameters = 2;
	RootDesc.NumStaticSamplers = 1;
	RootDesc.pParameters = RootParam;
	RootDesc.pStaticSamplers = &Sampler;
	RootDesc.Flags = flag;

	ComPtr<ID3DBlob> pBlob;
	ComPtr<ID3DBlob> pErrorBlob;

	// �V���A���C�Y
	hr = D3D12SerializeRootSignature(&RootDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		pBlob.GetAddressOf(), pErrorBlob.ReleaseAndGetAddressOf());
	if (FAILED(hr)) return false;

	// ���[�g�V�O�j�`���𐶐�
	hr = Device->CreateRootSignature(0, pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(), IID_PPV_ARGS(m_RootSignature.ReleaseAndGetAddressOf()));
	if (FAILED(hr)) return false;

	D3D12_INPUT_ELEMENT_DESC VertexLayout[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT	  ,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{ "COLOR"  , 0, DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"NORMAL"  , 0,DXGI_FORMAT_R32G32B32_FLOAT	  ,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT	  ,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
	};

	// �����_�[�^�[�Q�b�g�̃u�����h�ݒ�.
	D3D12_RENDER_TARGET_BLEND_DESC RTBDesc = {
		FALSE, FALSE,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_LOGIC_OP_NOOP,
		D3D12_COLOR_WRITE_ENABLE_ALL
	};

	// �u�����h�X�e�[�g�̐ݒ�.
	D3D12_BLEND_DESC BSDesc;
	BSDesc.AlphaToCoverageEnable = true;
	BSDesc.IndependentBlendEnable = FALSE;
	for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
	{
		BSDesc.RenderTarget[i] = RTBDesc;
	}

	// �[�x�X�e���V���X�e�[�g�ݒ�
	D3D12_DEPTH_STENCIL_DESC DepStencilDesc = {};
	DepStencilDesc.DepthEnable = false;
	DepStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	DepStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	DepStencilDesc.StencilEnable = false;

	ComPtr<ID3DBlob> VSBlob;
	ComPtr<ID3DBlob> PSBlob;

	// ���_�V�F�[�_�[�ǂݍ���
	if (!CompileShader("Shader/SimpleVS.hlsl", "main", "vs_5_0", VSBlob.ReleaseAndGetAddressOf()))
	{
		return false;
	}

	// �s�N�Z���V�F�[�_�[�ǂݍ���
	if (!CompileShader("Shader/TexturePS.hlsl", "main", "ps_5_0", PSBlob.ReleaseAndGetAddressOf()))
	{
		return false;
	}

	// �p�C�v���C���X�e�[�g�ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC PipeStateDesc = {};
	PipeStateDesc.InputLayout = { VertexLayout, _countof(VertexLayout) };
	PipeStateDesc.pRootSignature = m_RootSignature.Get();
	PipeStateDesc.VS.BytecodeLength = VSBlob->GetBufferSize();
	PipeStateDesc.VS.pShaderBytecode = VSBlob->GetBufferPointer();
	PipeStateDesc.PS.BytecodeLength = PSBlob->GetBufferSize();
	PipeStateDesc.PS.pShaderBytecode = PSBlob->GetBufferPointer();

	// ���X�^���C�U�[�X�e�[�g�ݒ�
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

	// �p�C�v���C���X�e�[�g����
	hr = Device->CreateGraphicsPipelineState(&PipeStateDesc, IID_PPV_ARGS(m_PSO.ReleaseAndGetAddressOf()));
	if (FAILED(hr)) return false;

	// �e�N�X�`������
	CreateTexture(Device);

	return true;
}
