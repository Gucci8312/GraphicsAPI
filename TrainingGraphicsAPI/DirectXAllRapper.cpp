#include "DirectXAllRapper.h"

ComPtr<ID3D11Device> DirectXAllRapper::Dx11Device;
ComPtr<ID3D12Device> DirectXAllRapper::Dx12Device;
uint32_t             DirectXAllRapper::m_FrameIndex;
ComPtr<ID3D12GraphicsCommandList>    DirectXAllRapper::m_CmdList;

// シェーダーコンパイル
bool CompileShader(const char* FileName, const char* EntryPoint, const char* ShaderVer, ID3DBlob** ReturnBlob)
{
#ifdef _DEBUG

	UINT    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT    compileFlags = 0;
#endif

	WCHAR	ConvertFileName[512];
	size_t 	wLen = 0;
	int err = 0;

	// char -> wcharに変換
	setlocale(LC_ALL, "japanese");
	err = mbstowcs_s(&wLen, ConvertFileName, 512, FileName, _TRUNCATE);

	ComPtr<ID3DBlob> pErrorBlob = NULL;
	if (FAILED(D3DCompileFromFile(ConvertFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, EntryPoint, ShaderVer, compileFlags, 0, ReturnBlob, pErrorBlob.ReleaseAndGetAddressOf())))
	{
		MessageBox(NULL, (char*)pErrorBlob->GetBufferPointer(), "Compile Error", MB_OK);
		pErrorBlob->Release();
		pErrorBlob = nullptr;
		return false;
	}

	return true;
}

HRESULT DirectX11Util::CreateBuffer(ID3D11Device* Device, D3D11_BIND_FLAG Flag, const void* Data, UINT Size, ID3D11Buffer** Buffer)
{
	//バッファ作成
	D3D11_BUFFER_DESC Desc;
	Desc.ByteWidth = Size;
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = Flag;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;
	Desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA ResouceData;
	ResouceData.pSysMem = Data;
	ResouceData.SysMemPitch = 0;
	ResouceData.SysMemSlicePitch = 0;

	// バッファ生成
	return Device->CreateBuffer(&Desc, &ResouceData, Buffer);
}

HRESULT DirectX11Util::CreateBuffer(ID3D11Device* Device, D3D11_BIND_FLAG Flag, UINT Size, ID3D11Buffer** Buffer)
{
	//バッファ設定
	D3D11_BUFFER_DESC Desc;
	Desc.ByteWidth = Size;
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = Flag;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;
	Desc.StructureByteStride = 0;

	// バッファ生成
	return Device->CreateBuffer(&Desc, NULL, Buffer);
}

HRESULT DirectX12Util::CreateBuffer(ID3D12Device* Device, UINT Size, ID3D12Resource** Buffer)
{
	// 頂点バッファ設定
	D3D12_HEAP_PROPERTIES Prop = {};
	Prop.Type = D3D12_HEAP_TYPE_UPLOAD;
	Prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	Prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	Prop.CreationNodeMask = 1;
	Prop.VisibleNodeMask = 1;

	// リソース設定
	D3D12_RESOURCE_DESC ResDesc = {};
	ResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResDesc.Alignment = 0;
	ResDesc.Width = Size;
	ResDesc.Height = 1;
	ResDesc.DepthOrArraySize = 1;
	ResDesc.MipLevels = 1;
	ResDesc.Format = DXGI_FORMAT_UNKNOWN;
	ResDesc.SampleDesc.Count = 1;
	ResDesc.SampleDesc.Quality = 0;
	ResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	ResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// バッファ生成
	return Device->CreateCommittedResource(&Prop, D3D12_HEAP_FLAG_NONE,
		&ResDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(Buffer));
}
