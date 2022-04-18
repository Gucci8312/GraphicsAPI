#pragma once
#include "Renderer.h"
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

class Dx12Renderer : public Renderer
{
	void startCommand(void* CmdObj)override;
	void SetRenderTarget();
	void ClearScreen(float BackColor,void* CmdObj);
	void ClearDepthView();
	void SetScreenParam();
};

