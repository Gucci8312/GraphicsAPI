#pragma once
#include <iostream>
#include <cstdint>

#include "AllInWrapper.h"
#include "DirectX11Wrapper.h"
#include "DirectX12Wrapper.h"

#define		NAME			"APIPractice"
#define		TITLE			"Renderer"

// アプリケーション管理クラス
class Application
{
private:
	HINSTANCE   m_hInst = nullptr;
	HWND        m_hWnd = nullptr;
	LONG		m_Width;
	LONG		m_Height;

	void Update();
	void Render();
	DirectXAllRapper* ApiWrapper = nullptr;
public:
	//void (Application::* Process)();
	bool Initialize();
	void Finalize();

	Application(uint32_t Width, uint32_t Height);
	~Application();
	bool InitWnd();
	void Run();
	void MainLoop();
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
};

