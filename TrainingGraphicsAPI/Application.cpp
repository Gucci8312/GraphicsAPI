#include "Application.h"

XMFLOAT3 CameraPos = { -0.0f,20.0f,60.0f };
DWORD			timeBefore;
Object Cube[1000];

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// 頂点リスト
Vertex VertexList[]
{
	// 前面
	{{-0.5f,-0.5f, 0.5f},{1.0f, 0.0f, 0.0f, 1.0f} ,{ 0.0f, 0.0f,-1.0f},{0.0f,0.0f}},
	{{-0.5f, 0.5f, 0.5f},{1.0f, 0.0f, 0.0f, 1.0f} ,{ 0.0f, 0.0f,-1.0f},{1.0f,0.0f}},
	{{ 0.5f,-0.5f, 0.5f},{1.0f, 0.0f, 0.0f, 1.0f} ,{ 0.0f, 0.0f,-1.0f},{0.0f,1.0f}},
	{{ 0.5f, 0.5f, 0.5f},{1.0f, 0.0f, 0.0f, 1.0f} ,{ 0.0f, 0.0f,-1.0f},{1.0f,1.0f}},

	// 後面
	{{-0.5f,-0.5f,-0.5f},{ 0.0f, 1.0f, 1.0f, 1.0f},{ 0.0f, 0.0f, 1.0f},{0.0f,0.0f}},
	{{-0.5f, 0.5f,-0.5f},{ 0.0f, 1.0f, 1.0f, 1.0f},{ 0.0f, 0.0f, 1.0f},{1.0f,0.0f}},
	{{ 0.5f,-0.5f,-0.5f},{ 0.0f, 1.0f, 1.0f, 1.0f},{ 0.0f, 0.0f, 1.0f},{0.0f,1.0f}},
	{{ 0.5f, 0.5f,-0.5f},{ 0.0f, 1.0f, 1.0f, 1.0f},{ 0.0f, 0.0f, 1.0f},{1.0f,1.0f}},

	// 右面
	{{ 0.5f, 0.5f, 0.5f},{ 1.0f, 1.0f, 0.0f, 1.0f},{-1.0f, 0.0f, 0.0f},{0.0f,0.0f}},
	{{ 0.5f, 0.5f,-0.5f},{ 1.0f, 1.0f, 0.0f, 1.0f},{-1.0f, 0.0f, 0.0f},{1.0f,0.0f}},
	{{ 0.5f,-0.5f, 0.5f},{ 1.0f, 1.0f, 0.0f, 1.0f},{-1.0f, 0.0f, 0.0f},{0.0f,1.0f}},
	{{ 0.5f,-0.5f,-0.5f},{ 1.0f, 1.0f, 0.0f, 1.0f},{-1.0f, 0.0f, 0.0f},{1.0f,1.0f}},

	// 左面
	{{-0.5f, 0.5f, 0.5f},{ 0.0f, 0.0f, 1.0f, 1.0f},{ 1.0f, 0.0f, 0.0f},{0.0f,0.0f}},
	{{-0.5f,-0.5f, 0.5f},{ 0.0f, 0.0f, 1.0f, 1.0f},{ 1.0f, 0.0f, 0.0f},{1.0f,0.0f}},
	{{-0.5f, 0.5f,-0.5f},{ 0.0f, 0.0f, 1.0f, 1.0f},{ 1.0f, 0.0f, 0.0f},{0.0f,1.0f}},
	{{-0.5f,-0.5f,-0.5f},{ 0.0f, 0.0f, 1.0f, 1.0f},{ 1.0f, 0.0f, 0.0f},{1.0f,1.0f}},


	// 上面
	{{-0.5f, 0.5f, 0.5f},{ 0.0f, 1.0f, 0.0f, 1.0f},{ 0.0f,-1.0f, 0.0f},{0.0f,0.0f}},
	{{-0.5f, 0.5f,-0.5f},{ 0.0f, 1.0f, 0.0f, 1.0f},{ 0.0f,-1.0f, 0.0f},{1.0f,0.0f}},
	{{ 0.5f, 0.5f, 0.5f},{ 0.0f, 1.0f, 0.0f, 1.0f},{ 0.0f,-1.0f, 0.0f},{0.0f,1.0f}},
	{{ 0.5f, 0.5f,-0.5f},{ 0.0f, 1.0f, 0.0f, 1.0f},{ 0.0f,-1.0f, 0.0f},{1.0f,1.0f}},

	// 下面
	{{-0.5f,-0.5f, 0.5f},{ 1.0f, 0.0f, 1.0f, 1.0f},{ 0.0f, 1.0f, 0.0f},{0.0f,0.0f}},
	{{ 0.5f,-0.5f, 0.5f},{ 1.0f, 0.0f, 1.0f, 1.0f},{ 0.0f, 1.0f, 0.0f},{1.0f,0.0f}},
	{{-0.5f,-0.5f,-0.5f},{ 1.0f, 0.0f, 1.0f, 1.0f},{ 0.0f, 1.0f, 0.0f},{0.0f,1.0f}},
	{{ 0.5f,-0.5f,-0.5f},{ 1.0f, 0.0f, 1.0f, 1.0f},{ 0.0f, 1.0f, 0.0f},{1.0f,1.0f}},
};

// インデックスリスト
unsigned int IndexList[]
{
	 0,  1,  2,     3,  2,  1,	// 前面
	 6,  5,  4,     5,  6,  7,  // 後面
	 8,  9, 10,    11, 10,  9,	// 右面
	12, 13, 14,    15, 14, 13,	// 左面
	16, 17, 18,    19, 18, 17,	// 上面
	20, 21, 22,    23, 22, 21,	// 下面
};

Application::Application(uint32_t Width, uint32_t Height)
{
	m_Width = Width;
	m_Height = Height;
	Process = nullptr;
}

Application::~Application()
{
}

void ThreadObjectInit(int ObjIdx, ID3D12Device* Device)
{
	for (int Idx = 0; Idx < 250; Idx++)
	{
		Cube[ObjIdx + Idx].PosSet(-125.0f + 3.0f * Idx, 10 - ObjIdx / 5, 0.0f);
		Cube[ObjIdx + Idx].ObjectCreate(Device, VertexList, ARRAYSIZE(VertexList), IndexList, ARRAYSIZE(IndexList));
	}
}

void ThreadObjectUpdate(int ObjIdx, ID3D12Device* Device, int FrameIndex)
{
	for (int Idx = 0; Idx < 250; Idx++)
	{
		Cube[ObjIdx + Idx].Update(FrameIndex);
	}
}

void ThreadObjectDraw(int ObjIdx, ID3D12GraphicsCommandList* CmdList, int FrameIndex)
{
	for (int Idx = 0; Idx < 250; Idx++)
	{
		Cube[ObjIdx + Idx].Draw(CmdList, FrameIndex);
	}
}

// ウィンドウの初期化
bool Application::InitWnd()
{
	// インスタンスハンドルを取得
	auto hInst = GetModuleHandle(nullptr);
	if (hInst == nullptr)
	{
		return false;
	}

	DWORD timeBefore;

	// メモリリークを検知
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	WNDCLASSEX		wcex;								// ウインドウクラス構造体

	// ウインドウクラス情報のセット
	wcex.hInstance = m_hInst;
	wcex.lpszClassName = NAME;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.style = 0;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.hIcon = LoadIcon((HINSTANCE)NULL, IDI_APPLICATION);
	wcex.hIconSm = LoadIcon((HINSTANCE)NULL, IDI_WINLOGO);
	wcex.hCursor = LoadCursor((HINSTANCE)NULL, IDC_ARROW);
	wcex.lpszMenuName = 0;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

	// ウインドウクラスの登録
	if (!RegisterClassEx(&wcex)) return FALSE;

	// インスタンスハンドル設定
	m_hInst = hInst;

	// ウィンドウのサイズを設定
	RECT rc = {};
	rc.right = static_cast<LONG>(m_Width);
	rc.bottom = static_cast<LONG>(m_Height);

	// ウィンドウサイズを調整
	auto style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
	AdjustWindowRect(&rc, style, FALSE);

	m_hWnd = CreateWindow(NAME, TITLE, WS_CAPTION | WS_SYSMENU, 0,
		0, rc.right - rc.left, rc.bottom - rc.top, HWND_DESKTOP, (HMENU)NULL, m_hInst, (LPVOID)NULL);

	if (!m_hWnd) return false;

	timeBefore = (DWORD)GetTickCount64();

	// タイマーセット
	SetTimer(m_hWnd, TIMER_ID, FREAM_RATE, NULL);


	ShowWindow(m_hWnd, SW_SHOWNORMAL);
	UpdateWindow(m_hWnd);

	// ウィンドウにフォーカスを設定
	SetFocus(m_hWnd);

	// 正常終了.
	return true;
}

// アプリケーションの実行
void Application::Run()
{
	if (Initialize())		// 初期化
	{
		MainLoop();			// メインループ
	}

	Finalize();				// 終了
}

// メインループ
void Application::MainLoop()
{
	MSG msg = {};

	// メッセージループ
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				Update();

				Render();
			}
		}
		else
		{
			Sleep(5);
		}
	}
}

// 初期化処理
bool Application::Initialize()
{
	std::cout << "ウィンドウタイプを選んでください" << std::endl;
	std::cout << "WindowsAPI >> 0" << std::endl;
	std::cout << "GLFW >> 1" << std::endl;

	// どのScreenAPIを使用するか選択
	int ChoiceScreenAPIIdx, ChoiceAPIIdx = 0;
	while (true)
	{
		std::cin >> ChoiceScreenAPIIdx;
		//if (ChoiceScreenAPIIdx <= 1)
		if (ChoiceScreenAPIIdx == 0)
		{
			break;
		}
	}

	system("cls");
	if (ChoiceScreenAPIIdx == 0)
	{
		Process = &Application::InitWnd;			// ウィンドウズAPI
		std::cout << "グラフィックAPIを選んでください" << std::endl;
		std::cout << "DirectX11 >> 0" << std::endl;
		std::cout << "DirectX12 >> 1" << std::endl;
	}
	else
	{
		Process = &Application::InitWnd;			// GLFW
		ChoiceAPIIdx += 2;
		std::cout << "グラフィックAPIを選んでください" << std::endl;
		std::cout << "OpenGL >> 0" << std::endl;
		std::cout << "Vulkan >> 1" << std::endl;
	}


	// どのグラフィックAPIを使用するか選択
	while (true)
	{
		std::cin >> ChoiceAPIIdx;
		if (ChoiceAPIIdx <= 1)
		{
			break;
		}
	}

	// コンソール画面終了
	FreeConsole();

	// API初期化分岐
	switch (ChoiceAPIIdx)
	{
	case 0:													// DirectX11 
		ApiWrapper.reset(new DirectX11Wrapper);
		break;
	case 1:													// DirectX12
		ApiWrapper.reset(new DirectX12Wrapper);
		//Cube.ObjectCreate(ApiWrapper->GetDevice(), VertexList, ARRAYSIZE(VertexList), IndexList, ARRAYSIZE(IndexList));
		break;
	case 2:													// OpenGL 
		ApiWrapper.reset(new DirectX11Wrapper);
		break;
	case 3:													// Vulkan
		ApiWrapper.reset(new DirectX12Wrapper);
		break;

	default:
		return false;
		break;
	}

	// ウィンドウ初期化
	if (!(this->*Process)())
	{
		return false;
	}

	RECT rc = { 0,0,m_Width,m_Height };

	// カメラ
	XMFLOAT3 EyePos = { 0.0f, 0.0f, 5.0f };
	XMFLOAT3 TargetPos = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 UpVector = { 0.0f, 1.0f, 0.0f };

	// カメラ初期化
	Camera::GetInstance().Init(m_Width, m_Height, EyePos, TargetPos, UpVector, 1.0f, 100.0f);
	//API初期化
	HRESULT hr = ApiWrapper->Create(m_hWnd, rc);
	if (FAILED(hr)) return false;

	//HRESULT hr = Dx.Create(m_hWnd, rc);
	//if (FAILED(hr)) return false;

	//Cube.ObjectCreate(Dx.GetDevice(), VertexList, ARRAYSIZE(VertexList), IndexList, ARRAYSIZE(IndexList), Dx.GetDeviceContext());

	//Cube.QuadInit(Dx.GetDevice());


	std::thread m_Initthred1(ThreadObjectInit, 0, ApiWrapper->GetDevice());
	std::thread m_Initthred2(ThreadObjectInit, 250, ApiWrapper->GetDevice());
	//std::thread m_Initthred3(ThreadObjectInit, 500, ApiWrapper->GetDevice());
	//std::thread m_Initthred4(ThreadObjectInit, 750, ApiWrapper->GetDevice());

	m_Initthred1.join();
	m_Initthred2.join();
	//m_Initthred3.join();
	//m_Initthred4.join();



	timeBefore = (DWORD)GetTickCount64();

	// タイマーセット
	SetTimer(m_hWnd, TIMER_ID, FREAM_RATE, NULL);

	return true;
}

// 更新処理
void Application::Update()
{
	Camera::GetInstance().SetCameraPos(XMLoadFloat3(&CameraPos));
	Camera::GetInstance().Update();
	//Cube.Update(Dx.GetDeviceContext());
	std::thread m_Updatehred1(ThreadObjectUpdate, 0, ApiWrapper->GetDevice(), ApiWrapper->GetFrameIdx());
	std::thread m_Updatehred2(ThreadObjectUpdate, 250, ApiWrapper->GetDevice(), ApiWrapper->GetFrameIdx());
	//std::thread m_Updatehred3(ThreadObjectUpdate, 500, ApiWrapper->GetDevice(), ApiWrapper->GetFrameIdx());
	//std::thread m_Updatehred4(ThreadObjectUpdate, 750, ApiWrapper->GetDevice(), ApiWrapper->GetFrameIdx());

	m_Updatehred1.join();
	m_Updatehred2.join();
	//m_Updatehred3.join();
	//m_Updatehred4.join();
}

// 描画処理
void Application::Render()
{
	//if (ApiWrapper != nullptr)
	{
		//Dx.BeforeRender();
		ApiWrapper->BeforeRender();

		std::thread m_Drawthred1(ThreadObjectDraw, 0, ApiWrapper->GetCmdList(), ApiWrapper->GetFrameIdx());
		std::thread m_Drawthred2(ThreadObjectDraw, 250, ApiWrapper->GetCmdList(), ApiWrapper->GetFrameIdx());
		//std::thread m_Drawthred3(ThreadObjectDraw, 500,ApiWrapper->GetCmdList(), ApiWrapper->GetFrameIdx());
		//std::thread m_Drawthred4(ThreadObjectDraw,750, ApiWrapper->GetCmdList(), ApiWrapper->GetFrameIdx());

		m_Drawthred1.join();
		m_Drawthred2.join();
		//m_Drawthred3.join();
		//m_Drawthred4.join();

		//Dx.AfterRender();
		ApiWrapper->AfterRender();
	}
}

// 終了処理
void Application::Finalize()
{
	if (ApiWrapper != nullptr)
	{
		//Dx.Release();
		ApiWrapper->Release();
	}
	// タイマー開放
	KillTimer(m_hWnd, TIMER_ID);
}

// ウィンドウプロシージャ
LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	case VK_UP:
		CameraPos.z += 1.0f;
		break;
	case VK_F2:
		CameraPos.z -= 1.0f;
		break;
	}
	break;

	default:
	{
	}
	break;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
