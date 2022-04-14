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

	float m_Width = 0;
	float m_Height = 0;
	float m_Near = 0;
	float m_Far = 0;
	float m_Offset = 10;
	XMMATRIX m_ViewMat = {};
	XMMATRIX m_ProjMat = {};
	XMVECTOR m_EyePos = {};
	XMVECTOR m_TargetPos = {};
	XMVECTOR m_UpVector={};

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
	bool Init(long Width, long Height, XMFLOAT3 CameraPos, XMFLOAT3 TargetPos, XMFLOAT3 UpVector, float Near, float Far);

	void Update();
	void UpdateSphereCamera(float Radius, float Elevation, float azimuth, const XMFLOAT3& lookat);

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
	
	void SetOffset(float Offset)
	{
		m_Offset = Offset;
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
		XMStoreFloat4x4(&ReturnMat, DirectX::XMMatrixLookAtRH(m_EyePos, m_TargetPos, m_UpVector));
		return ReturnMat;
	}

	XMFLOAT4X4 GetProjMatrix()
	{
		XMFLOAT4X4 ReturnMat;
		XMStoreFloat4x4(&ReturnMat, m_ProjMat);
		return ReturnMat;
	}
};



