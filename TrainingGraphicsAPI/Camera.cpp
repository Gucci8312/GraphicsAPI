#include "Camera.h"

// �C���X�^���X����
bool Camera::Init(float Width, float Height, XMFLOAT3 CameraPos, XMFLOAT3 TargetPos, XMFLOAT3 UpVector, float Near, float Far)
{
	m_Width = Width;
	m_Height = Height;
	SetNear(Near);
	SetFar(Far);

	// �J����������
	XMVECTOR TempVector;
	SetCameraPos(XMLoadFloat3(&CameraPos));
	SetTargetPos(XMLoadFloat3(&TargetPos));
	SetUpVector(XMLoadFloat3(&UpVector));

	// �r���[�ϊ�
	m_ViewMat = DirectX::XMMatrixLookAtRH(m_EyePos, m_TargetPos, m_UpVector);

	// �v���W�F�N�V�����ϊ�
	constexpr auto fovY = DirectX::XMConvertToRadians(45.0f);
	auto aspect = static_cast<float>(m_Width) / static_cast<float>(m_Height);
	m_ProjMat = DirectX::XMMatrixPerspectiveFovRH(fovY, aspect, m_Near, m_Far);

	return true;
}

// �J�����X�V
void Camera::Update()
{
	// �r���[�ϊ�
	m_ViewMat = DirectX::XMMatrixLookAtRH(m_EyePos, m_TargetPos, m_UpVector);

	// �v���W�F�N�V�����ϊ�
	constexpr auto fovY = DirectX::XMConvertToRadians(45.0f);
	auto aspect = static_cast<float>(m_Width) / static_cast<float>(m_Height);
	m_ProjMat = DirectX::XMMatrixPerspectiveFovRH(fovY, aspect, m_Near, m_Far);
}
