#pragma once
#include <Windows.h>
#include <cstdint>

class Application
{
private:
	// Member
	HINSTANCE	m_hInstance;
	HWND		m_hWnd;
	uint32_t	m_Width;
	uint32_t	m_Height;

	// Method
	bool InitApp();
	void TermApp();
	bool InitWnd();
	void TermWnd();
	void MainLoop();
	static LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp);

public:
	Application(uint32_t Width,uint32_t Height);
	~Application();
	void Run();
};

