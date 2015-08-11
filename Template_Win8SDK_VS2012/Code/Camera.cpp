#include "Camera.h"
#include "InputClass.h"
std::vector<Camera*> Camera::m_instance = std::vector<Camera*>(10);
Camera::Camera(){}
Camera::~Camera(){}
Camera* Camera::GetInstance(int p_index)
{
	if (m_instance.at(p_index) == nullptr)
	{
		m_instance.at(p_index) = new Camera;
		m_instance.at(p_index)->Initialize();
	}
	return m_instance.at(p_index);
}
void Camera::Initialize()
{
	m_cameraPosition = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_right = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	m_up = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	m_look = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	DirectX::XMStoreFloat4x4(&m_projectionMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_viewMatrix, DirectX::XMMatrixIdentity());
	SetLens(DirectX::XM_PIDIV4, 1.0f, 1.0f, 1000.0f);
	m_moveSpeed = 0.0f;

}
void Camera::Update(float p_deltaTime, int p_index)
{
	m_moveSpeed = p_deltaTime * 0.75f;

	m_instance[p_index]->UpdateKeyboard();
	m_instance[p_index]->UpdateMouse();
	m_instance[p_index]->UpdateViewMatrix();
}
void Camera::UpdateKeyboard()
{
	if (InputClass::GetInstance()->IsKeyPressed(VkKeyScan('a'))){ m_cameraPosition.x += m_moveSpeed; }
	if (InputClass::GetInstance()->IsKeyPressed(VkKeyScan('d'))){ m_cameraPosition.x -= m_moveSpeed; }
	if (InputClass::GetInstance()->IsKeyPressed(VkKeyScan('e'))){ m_cameraPosition.y += m_moveSpeed; }
	if (InputClass::GetInstance()->IsKeyPressed(VkKeyScan('q'))){ m_cameraPosition.y -= m_moveSpeed; }
	if (InputClass::GetInstance()->IsKeyPressed(VkKeyScan('w'))){ m_cameraPosition.z += m_moveSpeed; }
	if (InputClass::GetInstance()->IsKeyPressed(VkKeyScan('s'))){ m_cameraPosition.z -= m_moveSpeed; }

}
void Camera::UpdateMouse()
{
	if (InputClass::GetInstance()->IsLeftMousePressed())
	{
		m_look.z += m_moveSpeed;
	}
	if (InputClass::GetInstance()->IsRightMouseClicked())
	{
		m_look.z -= m_moveSpeed;
	}
}
void Camera::Shutdown(int p_index)
{
	if (m_instance[p_index] != nullptr)
	{
		delete m_instance[p_index];
		m_instance[p_index] = 0;
	}
}



void Camera::SetPosition(DirectX::XMFLOAT4 p_position)
{
	m_cameraPosition = p_position;
}
void Camera::SetLens(float p_fovy, float p_aspect, float p_zNear, float p_zFar)
{
	DirectX::XMMATRIX temp = DirectX::XMMatrixPerspectiveFovLH(p_fovy, p_aspect, p_zNear, p_zFar);
	DirectX::XMStoreFloat4x4(&m_projectionMatrix, temp);
}
void Camera::UpdateViewMatrix()
{
	DirectX::XMMATRIX temp = DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat4(&m_cameraPosition), DirectX::XMLoadFloat4(&m_look), DirectX::XMLoadFloat4(&m_up));
	DirectX::XMStoreFloat4x4(&m_viewMatrix, temp);
}
DirectX::XMFLOAT4 Camera::GetCameraPos()
{
	return m_cameraPosition;
}
DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return m_projectionMatrix;
}
DirectX::XMFLOAT4X4 Camera::GetViewMatrix()
{
	return m_viewMatrix;
}