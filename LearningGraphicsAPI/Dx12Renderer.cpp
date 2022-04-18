#include "Dx12Renderer.h"
// staticcastはコンパイル時に型整合するため実行時コストがない
void Dx12Renderer::startCommand(void* CmdObj)
{
	auto CmdList= static_cast<ID3D12GraphicsCommandList*>(CmdObj);
	
}

void Dx12Renderer::SetRenderTarget()
{
}

void Dx12Renderer::ClearScreen(float BackColor, void* CmdObj)
{
	auto CmdList = static_cast<ID3D12GraphicsCommandList*>(CmdObj);

}

void Dx12Renderer::ClearDepthView()
{
}

void Dx12Renderer::SetScreenParam()
{
}
