#include "Dx12Renderer.h"
// staticcast�̓R���p�C�����Ɍ^�������邽�ߎ��s���R�X�g���Ȃ�
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
