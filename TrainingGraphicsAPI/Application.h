#pragma once
#include <iostream>
#include <cstdint>
#include <memory>
#include <thread>

#include "AllInWrapper.h"
#include "DirectX11Wrapper.h"
#include "DirectX12Wrapper.h"
#include "Object.h"
#include "Camera.h"

#define		NAME			"APIPractice"
#define		TITLE			"Renderer"
#define		TIMER_ID 1
#define		FREAM_RATE (1000 / 60)

// アプリケーション管理クラス
class Application
{
private:
	HINSTANCE   m_hInst = nullptr;
	HWND        m_hWnd = nullptr;
	LONG		m_Width;
	LONG		m_Height;
	Object		m_Cube;

	//DirectX11Wrapper Dx;
	//DirectX12Wrapper Dx;
	Object      Quad;

	void Update();
	void Render();
	std::unique_ptr<DirectXAllRapper> ApiWrapper;

public:
	bool (Application::* Process)() = nullptr;
	bool Initialize();
	void Finalize();

	Application(uint32_t Width, uint32_t Height);
	~Application();
	bool InitWnd();
	void Run();
	void MainLoop();
};

