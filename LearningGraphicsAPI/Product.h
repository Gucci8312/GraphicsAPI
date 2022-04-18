#pragma once
#include <iostream>
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

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")


class GraphicDevice
{
public:
	virtual ~GraphicDevice() {};

	virtual GraphicDevice* GetDevice() = 0;
};

// DirectX11Device
class Dx11Device :public GraphicDevice
{
private:
	ID3D11Device*	Dx11Device;
public:
	GraphicDevice* GetDevice() { return (GraphicDevice*)Dx11Device; }
};

// DirectX12Device
class Dx12Device :public GraphicDevice
{
private:
	ID3D12Device* Dx12Device;
public:
	GraphicDevice* GetDevice() { return (GraphicDevice*)Dx12Device; }
};

// ファクトリーパターン
class Product
{
public:
	virtual GraphicDevice* Use() = 0;
};

class Factory
{
public:
	virtual Product* Create(Dx11Device* Dx11Device) final;
	virtual Product* Create(Dx12Device* Dx12Device) final;

	virtual Product* CreateProduct(Dx11Device* Dev) = 0;
	virtual Product* CreateProduct(Dx12Device* Dev) = 0;

	//virtual void RegisterProduct(Product* Product) = 0;
};