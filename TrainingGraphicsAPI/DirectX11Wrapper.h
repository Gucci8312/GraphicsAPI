#pragma once
#include "DirectXAllRapper.h"

// DirectX11èâä˙âªÉNÉâÉX
class DirectX11Wrapper :public DirectXAllRapper
{
private:
	ComPtr<ID3D11Device>				m_Device;
	ComPtr<ID3D11DeviceContext>			m_ImmediateContext;
	ComPtr<IDXGISwapChain>				m_SwapChain;
	ComPtr<ID3D11RenderTargetView>		m_RenderTargetView;
	ComPtr<ID3D11Texture2D>				m_DepthStencilTexture;
	ComPtr<ID3D11DepthStencilView>		m_DepthStencilView;
	D3D11_VIEWPORT						m_ViewPort = {};
	ComPtr<ID3D11RasterizerState>		m_RasterState;

public:
	HRESULT Create(HWND hwnd, RECT rc)	override final;
	void    Release()					override final;
	void	BeforeRender()				override final;
	void	AfterRender()				override final;
	//ID3D11Device* GetDevice() { return m_Device.Get(); }
	ID3D11DeviceContext* GetDeviceContext() { return m_ImmediateContext.Get(); }
};