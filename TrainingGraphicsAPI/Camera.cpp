#include "Camera.h"

// ���W�A���ɕϊ�
float ToRad(float _Angle)
{
	return _Angle * (3.14f / 180.0f);
}

// �v���C���[�̎������]����J����
void Camera::UpdateSphereCamera(float Radius, float Elevation, float azimuth, const XMFLOAT3& lookat)
{
	XMFLOAT3 eye;// ���_

	// �p�A���ʊp�����W�A���ɕϊ�
	Elevation = ToRad(Elevation);
	azimuth = ToRad(azimuth);

	// �p�ɉ��������a���v�Z
	float r = Radius * sinf(Elevation);

	// ���_���W���v�Z�i3D�ɍ��W�n���f�J���g���W�n�֕ϊ��j
	eye.x = r * cosf(azimuth);
	eye.y = Radius * cosf(Elevation);
	eye.z = r * sinf(azimuth);

	// ������x�N�g�����v�Z�i�p��90�x��]������ƌv�Z�ł���j
	DirectX::XMFLOAT3 up;
	float ElevationPlus90 = Elevation + ToRad(90.0f);

	// ������x�N�g�����v�Z�i3D�ɍ��W�n���f�J���g���W�n�֕ϊ��j
	r = Radius * sinf(ElevationPlus90);

	up.x = r * cosf(azimuth);
	up.y = Radius * cosf(ElevationPlus90);
	up.z = r * sinf(azimuth);

	//���_�x�N�g���𒍎��_�����s�ړ�
	eye.x += lookat.x;
	eye.y += lookat.y;
	eye.z += lookat.z;

	// �r���[�ϊ��s������
	SetCameraPos(XMLoadFloat3(&eye));
	SetTargetPos(XMLoadFloat3(&lookat));
	SetUpVector(XMLoadFloat3(&up));
}

// �C���X�^���X����
bool Camera::Init(long Width, long Height, XMFLOAT3 CameraPos, XMFLOAT3 TargetPos, XMFLOAT3 UpVector, float Near, float Far)
{
	m_Width = Width;
	m_Height = Height;
	SetNear(Near);
	SetFar(Far);

	// �J����������
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
	//XMFLOAT3 TempPos;
	//XMStoreFloat3(&TempPos,m_EyePos);
	//UpdateSphereCamera(TempPos.z, TempPos.x, TempPos.y, XMFLOAT3(0.0f, 0.0f, 1.0f));
	// �r���[�ϊ�
	m_ViewMat = DirectX::XMMatrixLookAtRH(m_EyePos, m_TargetPos, m_UpVector);

	// �v���W�F�N�V�����ϊ�
	constexpr auto fovY = DirectX::XMConvertToRadians(45.0f);
	auto aspect = static_cast<float>(m_Width) / static_cast<float>(m_Height);
	m_ProjMat = DirectX::XMMatrixPerspectiveFovRH(fovY, aspect, m_Near, m_Far);
}