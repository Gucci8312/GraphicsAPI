#include "main.h"

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

//#include "Application.h"
//
//int wmain(int argc, wchar_t** argv, wchar_t** evnp)
//{
//#if defined(DEBUG) || defined(_DEBUG)
//    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//#endif
//
//    // アプリケーション実行
//    Application app(SCREEN_WIDTH, SCREEN_HEIGHT);
//    app.Run();
//
//    return 0;
//}
//

XMFLOAT3 CameraPos = { 0.0f, 0.0f, 5.0f };
#define TIMER_ID 1
#define FREAM_RATE (1000 / 60)

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	#if defined(DEBUG) || defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	DirectX11Wrapper Directx;
	//DirectX12Wrapper Directx;

	HWND			hwnd;								// ウインドウハンドル
	MSG				msg;								// メッセージ構造体
	WNDCLASSEX		wcex;								// ウインドウクラス構造体
	DWORD			timeBefore;

	// メモリリーク検知
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// ウインドウクラス情報セット
	wcex.hInstance = hInstance;
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
	if (!RegisterClassEx(&wcex)) return false;


	RECT rc = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
	hwnd = CreateWindow(NAME, TITLE, WS_CAPTION | WS_SYSMENU, 0,
		0, rc.right - rc.left, rc.bottom - rc.top, HWND_DESKTOP, (HMENU)NULL, hInstance, (LPVOID)NULL);

	if (!hwnd) return false;

	timeBefore = (DWORD)GetTickCount64();

	// タイマーセット
	SetTimer(hwnd, TIMER_ID, FREAM_RATE, NULL);

	// DirectxX12の初期化
	if (FAILED(Directx.Create(hwnd, rc)))
	{
		return 0;
	}

	XMFLOAT3 EyePos = { 0.0f, 0.0f, 5.0f };
	XMFLOAT3 TargetPos = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 UpVector = { 0.0f, 1.0f, 0.0f };

	// カメラ初期化
	Camera::GetInstance().Init(SCREEN_WIDTH,SCREEN_HEIGHT, EyePos, TargetPos, UpVector, 1.0f, 100.0f);
	auto aa=Camera::GetInstance().GetProjMatrix();

	Directx.PolygonInit();
	//Directx.CubeInit();

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

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

				Camera::GetInstance().SetCameraPos(XMLoadFloat3(&CameraPos));
				Camera::GetInstance().Update();

				// DirectX 描画前処理
				Directx.BeforeRender();


				// オブジェクト描画
				Directx.ObjectDraw();

				// DirectX 描画後処理
				Directx.AfterRender();
			}
		}
		else
		{
			Sleep(5);
		}
	}

	// DirectX1終了処理
	Directx.Release();

	// タイマー開放
	KillTimer(hwnd, TIMER_ID);

	return (int)msg.wParam;
}

// ウィンドウプロシージャ
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;
		case VK_RIGHT:
			CameraPos.x += 0.1f;
			break;
		case VK_LEFT:
			CameraPos.x -= 0.1f;
			break;
		case VK_UP:
			CameraPos.y -= 0.1f;
			break;
		case VK_DOWN:
			CameraPos.y += 0.1f;
			break;
		case VK_F1:
			CameraPos.z += 0.1f;
			break;
		case VK_F2:
			CameraPos.z -= 0.1f;
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}