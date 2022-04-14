#pragma once

#include <d3d11.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <Directxmath.h>
#include <Windows.h> 
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <string>
#include <random>
#include <locale.h>

#include "Camera.h"


#pragma comment(lib,"d3d11.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

#define SAFE_RELEASE(p)      { if( NULL != p ) { p->Release(); p = NULL; } }
#define PI 3.141593

using namespace DirectX;
using namespace Microsoft::WRL;

namespace DirectX11Util
{
	HRESULT CreateBuffer(ID3D11Device* Device,D3D11_BIND_FLAG Flag,const void* Data, UINT Size, ID3D11Buffer** Buffer);
	HRESULT CreateBuffer(ID3D11Device* Device, D3D11_BIND_FLAG Flag, UINT Size, ID3D11Buffer** Buffer);
}

namespace DirectX12Util
{
	HRESULT CreateBuffer(ID3D12Device* Device , UINT Size, ID3D12Resource** Buffer);
}


struct Vertex {
	float pos[3];
	float col[4];
	float nor[3];
	float tex[2];
};

struct ConstantBuffer {
	XMFLOAT4X4 world;
	XMFLOAT4X4 view;
	XMFLOAT4X4 projection;
	XMFLOAT4   LightDir;
};
	bool CompileShader(const char* FileName, const char* EntryPoint, const char* ShaderVer, ID3DBlob** ReturnBlob);

class DirectXAllRapper
{
protected:
	static ComPtr<ID3D11Device> Dx11Device;
	static ComPtr<ID3D12Device> Dx12Device;
public:
	virtual HRESULT Create(HWND hwnd, RECT rc) = 0;
	virtual void    Release() = 0;
	virtual void	BeforeRender() = 0;
	virtual void	AfterRender() = 0;
	ID3D12Device* GetDevice() { return Dx12Device.Get(); }
};

