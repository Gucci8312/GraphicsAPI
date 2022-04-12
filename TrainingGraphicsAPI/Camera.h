#pragma once
#include <Directxmath.h>

using namespace DirectX;

// シングルトンパターン
// カメラ管理クラス
class Camera
{
private:
	Camera() = default;
	~Camera() = default;

	float m_Width;
	float m_Height;
	float m_Near;
	float m_Far;
	XMMATRIX m_ViewMat;
	XMMATRIX m_ProjMat;
	XMVECTOR m_EyePos;
	XMVECTOR m_TargetPos;
	XMVECTOR m_UpVector;

public:
	Camera(const Camera&) = delete;
	Camera& operator=(const Camera&) = delete;
	Camera(Camera&&) = delete;
	Camera& operator=(Camera&&) = delete;

	// インスタンス取得
	static Camera& GetInstance()
	{
		static Camera Instance;
		return Instance;
	}
	
	// インスタンス生成
	bool Init(float Width,float Height, XMFLOAT3 CameraPos, XMFLOAT3 TargetPos, XMFLOAT3 UpVector,float Near,float Far);

	void Update();

	void SetCameraPos(XMVECTOR Pos)
	{
		m_EyePos = Pos;
	}

	void SetTargetPos(XMVECTOR TargetPos)
	{
		m_TargetPos = TargetPos;
	}

	void SetUpVector(XMVECTOR UpVector)
	{
		m_UpVector = UpVector;
	}

	void SetNear(float Near)
	{
		m_Near = Near;
	}

	void SetFar(float Far)
	{
		m_Far = Far;
	}

	XMFLOAT4X4 GetViewMatrix()
	{
		XMFLOAT4X4 ReturnMat;
		XMStoreFloat4x4(&ReturnMat, m_ViewMat);
		return ReturnMat;
	}

	XMFLOAT4X4 GetProjMatrix()
	{
		XMFLOAT4X4 ReturnMat;
		XMStoreFloat4x4(&ReturnMat, m_ProjMat);
		return ReturnMat;
	}
};



