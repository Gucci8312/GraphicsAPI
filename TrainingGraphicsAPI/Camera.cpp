#include "Camera.h"

// ラジアンに変換
float ToRad(float _Angle)
{
	return _Angle * (3.14f / 180.0f);
}

// プレイヤーの周りを回転するカメラ
void Camera::UpdateSphereCamera(float Radius, float Elevation, float azimuth, const XMFLOAT3& lookat)
{
	XMFLOAT3 eye;// 視点

	// 仰角、方位角をラジアンに変換
	Elevation = ToRad(Elevation);
	azimuth = ToRad(azimuth);

	// 仰角に応じた半径を計算
	float r = Radius * sinf(Elevation);

	// 視点座標を計算（3D極座標系をデカルト座標系へ変換）
	eye.x = r * cosf(azimuth);
	eye.y = Radius * cosf(Elevation);
	eye.z = r * sinf(azimuth);

	// 上向きベクトルを計算（仰角を90度回転させると計算できる）
	DirectX::XMFLOAT3 up;
	float ElevationPlus90 = Elevation + ToRad(90.0f);

	// 上向きベクトルを計算（3D極座標系をデカルト座標系へ変換）
	r = Radius * sinf(ElevationPlus90);

	up.x = r * cosf(azimuth);
	up.y = Radius * cosf(ElevationPlus90);
	up.z = r * sinf(azimuth);

	//視点ベクトルを注視点分平行移動
	eye.x += lookat.x;
	eye.y += lookat.y;
	eye.z += lookat.z;

	// ビュー変換行列を作る
	SetCameraPos(XMLoadFloat3(&eye));
	SetTargetPos(XMLoadFloat3(&lookat));
	SetUpVector(XMLoadFloat3(&up));
}

// インスタンス生成
bool Camera::Init(long Width, long Height, XMFLOAT3 CameraPos, XMFLOAT3 TargetPos, XMFLOAT3 UpVector, float Near, float Far)
{
	m_Width = Width;
	m_Height = Height;
	SetNear(Near);
	SetFar(Far);

	// カメラ情報入力
	SetCameraPos(XMLoadFloat3(&CameraPos));
	SetTargetPos(XMLoadFloat3(&TargetPos));
	SetUpVector(XMLoadFloat3(&UpVector));

	// ビュー変換
	m_ViewMat = DirectX::XMMatrixLookAtRH(m_EyePos, m_TargetPos, m_UpVector);

	// プロジェクション変換
	constexpr auto fovY = DirectX::XMConvertToRadians(45.0f);
	auto aspect = static_cast<float>(m_Width) / static_cast<float>(m_Height);
	m_ProjMat = DirectX::XMMatrixPerspectiveFovRH(fovY, aspect, m_Near, m_Far);

	return true;
}

// カメラ更新
void Camera::Update()
{
	//XMFLOAT3 TempPos;
	//XMStoreFloat3(&TempPos,m_EyePos);
	//UpdateSphereCamera(TempPos.z, TempPos.x, TempPos.y, XMFLOAT3(0.0f, 0.0f, 1.0f));
	// ビュー変換
	m_ViewMat = DirectX::XMMatrixLookAtRH(m_EyePos, m_TargetPos, m_UpVector);

	// プロジェクション変換
	constexpr auto fovY = DirectX::XMConvertToRadians(45.0f);
	auto aspect = static_cast<float>(m_Width) / static_cast<float>(m_Height);
	m_ProjMat = DirectX::XMMatrixPerspectiveFovRH(fovY, aspect, m_Near, m_Far);
}