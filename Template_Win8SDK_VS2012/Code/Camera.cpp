#include "Camera.h"
#include "InputClass.h"
#include <WinUser.h>
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
	//m_cameraPosition = DirectX::XMFLOAT4(-1992.0f, -1764.0f, -1992.0f, 1.0f);
	m_cameraPosition = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_right = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	m_up = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	m_look = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	m_upDefault = m_up;
	m_rightDefault = m_right;
	m_lookDefault = m_look;
	m_yaw = DirectX::XM_PI;
	m_pitch = DirectX::XM_PI;

	DirectX::XMStoreFloat4x4(&m_projectionMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_viewMatrix, DirectX::XMMatrixIdentity());
	SetLens(3.1415f/4.0f, 1.0f, 1.0f, 10000.0f);
	m_moveSpeed = 0.0f;
	InputClass::GetInstance()->RegisterKey(VkKeyScan('i'));
	InputClass::GetInstance()->RegisterKey(VkKeyScan('j'));
	InputClass::GetInstance()->RegisterKey(VkKeyScan('k'));
	InputClass::GetInstance()->RegisterKey(VkKeyScan('l'));
	//InputClass::GetInstance()->RegisterKey(VkKeyScan(VK_LEFT));
	//InputClass::GetInstance()->RegisterKey(VkKeyScan(VK_RIGHT));
	//InputClass::GetInstance()->RegisterKey(VkKeyScan(VK_UP));
	//InputClass::GetInstance()->RegisterKey(VkKeyScan(VK_DOWN));
	InputClass::GetInstance()->RegisterKey(VkKeyScan(VK_SPACE));

}
void Camera::Update(float p_deltaTime, HWND p_hwnd, int p_screenW, int p_scrrenH)
{
	m_screenHeight = p_scrrenH;
	m_screenWidth = p_screenW;
	m_rotateSpeed = p_deltaTime * 0.75f;
	//m_moveSpeed = p_deltaTime * 1.0f;
	m_moveSpeed = p_deltaTime * 450.0f;

	m_instance->UpdateKeyboard();
	m_instance->UpdateMouse(p_hwnd);
	m_instance->UpdateViewMatrix();
}
void Camera::UpdateKeyboard()
{
	//Strafe
	if (InputClass::GetInstance()->IsKeyPressed(VkKeyScan('a')))
	{
		Strafe(m_moveSpeed);
	}
	if (InputClass::GetInstance()->IsKeyPressed(VkKeyScan('d')))
	{
		Strafe(-m_moveSpeed);
	}

	//Up and down
	if (InputClass::GetInstance()->IsKeyPressed(VkKeyScan('e')))
	{
		UpOrDown(m_moveSpeed);
	}
	if (InputClass::GetInstance()->IsKeyPressed(VkKeyScan('q')))
	{
		UpOrDown(-m_moveSpeed);
	}

	//Back and forward
	if (InputClass::GetInstance()->IsKeyPressed(VkKeyScan('w')))
	{
		Walk(m_moveSpeed);
	}
	if (InputClass::GetInstance()->IsKeyPressed(VkKeyScan('s')))
	{
		Walk(-m_moveSpeed);
	}


	//Rotate camera
	if (InputClass::GetInstance()->IsKeyPressed(VkKeyScan('i')))
	{
		Pitch(-m_rotateSpeed);

	}
	if (InputClass::GetInstance()->IsKeyPressed(VkKeyScan('k')))
	{
		Pitch(m_rotateSpeed);
	}
	if (InputClass::GetInstance()->IsKeyPressed(VkKeyScan('l')))
	{
		RotateY(m_rotateSpeed);
	}
	if (InputClass::GetInstance()->IsKeyPressed(VkKeyScan('j')))
	{
		RotateY(-m_rotateSpeed);
	}



	if (InputClass::GetInstance()->IsKeyPressed(VkKeyScan(VK_SPACE)))
	{
		m_cameraPosition = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		m_right = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		m_up = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		m_look = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

		DirectX::XMStoreFloat4x4(&m_projectionMatrix, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&m_viewMatrix, DirectX::XMMatrixIdentity());
		SetLens(3.1415f / 4.0f, 1.0f, 1.0f, 10000.0f);
	}

}
void Camera::Strafe(float p_distance)
{
	float x, y, z, w;
	x = m_cameraPosition.x + p_distance * m_right.x;
	y = m_cameraPosition.y + p_distance * m_right.y;
	z = m_cameraPosition.z + p_distance * m_right.z;
	w = m_cameraPosition.w;

	DirectX::XMFLOAT4 newCameraPos = DirectX::XMFLOAT4(x,y,z,w);

	m_cameraPosition = newCameraPos;	
}
void Camera::Walk(float p_distance)
{
	float x, y, z, w;
	x = m_cameraPosition.x + p_distance * m_look.x;
	y = m_cameraPosition.y + p_distance * m_look.y;
	z = m_cameraPosition.z + p_distance * m_look.z;
	w = m_cameraPosition.w;

	DirectX::XMFLOAT4 newCameraPos = DirectX::XMFLOAT4(x, y, z, w);

	m_cameraPosition = newCameraPos;
}
void Camera::UpOrDown(float p_distance)
{
	m_cameraPosition.y += p_distance;
	//float x, y, z, w;
	//x = m_cameraPosition.x + p_distance * m_up.x;
	//y = m_cameraPosition.y + p_distance/* * m_up.y*/;
	//z = m_cameraPosition.z + p_distance * m_up.z;
	//w = m_cameraPosition.w;

	//DirectX::XMFLOAT4 newCameraPos = DirectX::XMFLOAT4(x, y, z, w);

	//m_cameraPosition = newCameraPos;
}
void Camera::Pitch(float p_angle)
{
	DirectX::XMMATRIX rotation;
	rotation = DirectX::XMMatrixRotationX(p_angle);

	//DirectX::XMStoreFloat4(&m_up, DirectX::XMVector4Transform(DirectX::XMLoadFloat4(&m_up), rotation));
	DirectX::XMStoreFloat4(&m_look, DirectX::XMVector4Transform(DirectX::XMLoadFloat4(&m_look), rotation));
}
void Camera::RotateY(float p_angle)
{
	DirectX::XMMATRIX rotation;
	rotation = DirectX::XMMatrixRotationY(p_angle);

	DirectX::XMStoreFloat4(&m_right, DirectX::XMVector4Transform(DirectX::XMLoadFloat4(&m_right), rotation));
	//DirectX::XMStoreFloat4(&m_up, DirectX::XMVector4Transform(DirectX::XMLoadFloat4(&m_up), rotation));
	DirectX::XMStoreFloat4(&m_look, DirectX::XMVector4Transform(DirectX::XMLoadFloat4(&m_look), rotation));
}
void Camera::UpdateMouse(HWND p_hwnd)
{
	POINT p;
	if (GetCursorPos(&p))
	{
		if (ScreenToClient(p_hwnd, &p))
		{
			POINT mousePos;
			int dx, dy;
			mousePos.x = p.x;
			mousePos.y = p.y;
			dx = p.x - m_screenWidth;
			dy = p.y - m_screenHeight;

			m_yaw += dx * MOUSE_SPEED;
			m_pitch += -dy * MOUSE_SPEED;

			DirectX::XMMATRIX rotation;
			rotation = DirectX::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0.0f);

			DirectX::XMStoreFloat4(&m_right, DirectX::XMVector4Transform(DirectX::XMLoadFloat4(&m_rightDefault), rotation));
			DirectX::XMStoreFloat4(&m_look, DirectX::XMVector4Transform(DirectX::XMLoadFloat4(&m_lookDefault), rotation));

			p.x = m_screenWidth;
			p.y = m_screenHeight;
			ClientToScreen(p_hwnd, &p);
			SetCursorPos(p.x, p.y);
			ShowCursor(FALSE);
		}
	}
}
void Camera::Shutdown()
{
	if (m_instance != nullptr)
	{
		delete m_instance;
		m_instance = 0;
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

DirectX::XMFLOAT4 Camera::GetLookAt()
{
	return m_look;
}
DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return m_projectionMatrix;
}
DirectX::XMFLOAT4X4 Camera::GetViewMatrix()
{
	return m_viewMatrix;
}