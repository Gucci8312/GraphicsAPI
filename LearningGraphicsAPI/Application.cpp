#include "Application.h"

const auto ClassName = TEXT("LearningGraphicsAPI");

Application::Application(uint32_t Width, uint32_t Height) :
	m_hInstance(nullptr), m_hWnd(nullptr), m_Width(Width), m_Height(Height)
{
	/*Nothing*/
}

Application::~Application()
{
	/*Nothing*/
}

// Initlize Uplication
bool Application::InitApp()
{
	//Initlize Window
	if (!InitWnd())	return false;

	// Destroy Console
	FreeConsole();

	return true;
}

// Terminate Application
void Application::TermApp()
{
	// Destroy Window
	TermWnd();
}

// Initlize Window 
bool Application::InitWnd()
{
	auto hInst = GetModuleHandle(nullptr);
	if (hInst == nullptr)	return false;

	// Window Setting
	WNDCLASSEX wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hIcon = LoadIcon(hInst, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(hInst, IDC_ARROW);
	wcex.hbrBackground = GetSysColorBrush(COLOR_BACKGROUND);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = ClassName;
	wcex.hIconSm = LoadIcon(hInst, IDI_APPLICATION);

	// Register Window
	if (!RegisterClassEx(&wcex))	return false;

	// Set Instance Handle
	m_hInstance = hInst;

	// Setting Window Scale
	RECT rc = {};
	rc.right = static_cast<long>(m_Width);
	rc.bottom = static_cast<long>(m_Height);

	// Adjust Window Scale
	auto Style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
	AdjustWindowRect(&rc, Style, false);

	// Create Window
	m_hWnd = CreateWindowEx(0, ClassName, TEXT("Sample"), Style, CW_USEDEFAULT, CW_USEDEFAULT,
		rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, m_hInstance, nullptr);

	if (m_hWnd == nullptr)	return false;

	// Display Window
	ShowWindow(m_hWnd, SW_SHOWNORMAL);

	// Update 
	UpdateWindow(m_hWnd);

	// Focus Window
	SetFocus(m_hWnd);

	return true;
}

// Terminate Window
void Application::TermWnd()
{
	// ÉEÉBÉìÉhÉEÇÃìoò^Çâèú
	if (m_hInstance != nullptr)
	{
		UnregisterClass(ClassName, m_hInstance);
	}

	m_hInstance = nullptr;
	m_hWnd = nullptr;
}

// MessageLoop
void Application::MainLoop()
{
	MSG msg = {};

	while (WM_QUIT!=msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) == true)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

// Application Main
void Application::Run()
{
	if (InitApp())
	{
		MainLoop();
	}

	TermApp();
}

LRESULT CALLBACK Application::WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return DefWindowProc(hWnd, msg, wp, lp);
}