#include "Camera.h"

Camera* Camera::m_instance;
Camera::Camera(){}
Camera::~Camera(){}
Camera* Camera::GetInstance()
{
	if (m_instance == nullptr)
	{
		m_instance = new Camera;
		m_instance->Initialize();
	}
	return m_instance;
}
void Camera::Initialize()
{
	m_cameraPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
}
void Camera::Update(float p_deltaTime)
{

}
void Camera::Shutdown()
{
	if (m_instance != nullptr)
	{
		delete m_instance;
		m_instance = 0;
	}
}

DirectX::XMFLOAT3 Camera::GetCameraPos()
{
	return m_cameraPosition;
}
