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
	XMFLOAT3								m_Pos;
	ComPtr<ID3D12DescriptorHeap>			m_BasicDescHeap;
	ComPtr<ID3D12Resource>					m_VertexBuffer;
	ComPtr<ID3D12Resource>					m_IndexBuffer;
	ComPtr<ID3D12Resource>					m_ConstantBuffer[2 * 2];
	ComPtr<ID3D12RootSignature>				m_RootSignature;
	ComPtr<ID3D12PipelineState>				m_PSO;
	D3D12_VERTEX_BUFFER_VIEW				m_VBView = {};
	D3D12_INDEX_BUFFER_VIEW					m_IBView = {};
	ConstantBufferView<ConstantBuffer>		m_CBView[2 * 2];
	float									m_RotateAngle = 0.0f;
	int										m_IndexNum = 0;
	Texture									m_Texture = {};

public:
	void PosSet(float x, float y, float z) { m_Pos = { x,y,z }; }
	bool QuadInit(ID3D12Device* Device);
	bool CubeInit(ID3D12Device* Device);
	void Draw(ID3D12GraphicsCommandList* CmdList, int FrameIndex);
	void Update(int FrameIndex);
	bool CreateTexture(ID3D12Device* Device);
};

