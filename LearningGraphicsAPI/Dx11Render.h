#pragma once
#include "Renderer.h"
class Dx11Render :
	public Renderer
{
	void startCommand();
	void SetRenderTarget();
	void ClearRenderTarget(float BackColor) ;
	void ClearDepthView();
	void SetScreenParam();

};

