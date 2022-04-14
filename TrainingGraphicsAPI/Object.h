#pragma once
#include "DirectXAllRapper.h"

class Object
{
private:
	template<typename T>
	struct ConstantBufferView
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC Desc;
		D3D12_CPU_DESCRIPTOR_HANDLE     HandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE     HandleGPU;
		T* pBuffer;
	};

	struct Texture
	{
		ComPtr<ID3D12Resource> pResouce;
		D3D12_CPU_DESCRIPTOR_HANDLE HandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE HandleGPU;
	};

private:
	float									m_RotateAngle = 0.0f;
	XMFLOAT3								m_Pos;
	int										m_IndexNum = 0;

	// DX12
	ComPtr<ID3D12DescriptorHeap>			m_12BasicDescHeap;
	ComPtr<ID3D12Resource>					m_12VertexBuffer;
	ComPtr<ID3D12Resource>					m_12IndexBuffer;
	ComPtr<ID3D12Resource>					m_12ConstantBuffer[2 * 2];
	ComPtr<ID3D12RootSignature>				m_12RootSignature;
	ComPtr<ID3D12PipelineState>				m_12PSO;
	D3D12_VERTEX_BUFFER_VIEW				m_12VBView = {};
	D3D12_INDEX_BUFFER_VIEW					m_12IBView = {};
	ConstantBufferView<ConstantBuffer>		m_12CBView[2 * 2];
	Texture									m_12Texture = {};

	// DX11
	ComPtr<ID3D11Buffer>				m_11VertexBuffer;
	ComPtr<ID3D11Buffer>				m_11IndexBuffer;
	ComPtr<ID3D11Buffer>				m_11ConstantBuffer;
	ComPtr<ID3D11InputLayout>			m_11InputLayOut;
	ComPtr<ID3D11VertexShader>			m_11VertexShader;
	ComPtr<ID3D11PixelShader>			m_11PixelShader;
	ComPtr<ID3D11ShaderResourceView>	m_11NoiseTextureResouce;
	ComPtr<ID3D11SamplerState>			m_11SamplerState;
	int									m_11IndexNum = 0;

public:
	void PosSet(float x, float y, float z) { m_Pos = { x,y,z }; }
	bool ObjectCreate(ID3D11Device* Device,Vertex* VertexList, int VertexNum, unsigned short* IndexList,int IndexNum, ID3D11DeviceContext* Context);
	bool ObjectCreate(ID3D12Device* Device,Vertex* VertexList, int VertexNum, uint32_t* IndexList,int IndexNum);
	void Draw(ID3D12GraphicsCommandList* CmdList, int FrameIndex);
	void Draw(ID3D11DeviceContext* CmdList);
	void Update(int FrameIndex);
	void Update(ID3D11DeviceContext* Context);
	bool CreateTexture(ID3D12Device* Device);
	bool CreateTexture(ID3D11Device* Device, ID3D11DeviceContext* Context);
};

