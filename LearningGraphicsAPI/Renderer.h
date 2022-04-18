#pragma once

class Renderer
{
private:

public:
	virtual void startCommand(void* Data) = 0;
	virtual void SetRenderTarget() = 0;
	virtual void ClearRenderTarget(float BackColor) = 0;
	virtual void ClearDepthView() = 0;
	virtual void SetScreenParam() = 0;
};

