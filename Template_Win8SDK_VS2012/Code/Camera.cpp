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
	//m_cameraPosition = DirectX::XMFLOAT4(-1992.0f, -1764.0f, -1992.0f, 1.0f);
	m_cameraPosition = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_right = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	m_up = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	m_look = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	DirectX::XMStoreFloat4x4(&m_projectionMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_viewMatrix, DirectX::XMMatrixIdentity());
	SetLens(3.1415f/4.0f, 1.0f, 1.0f, 10000.0f);
	m_moveSpeed = 0.0f;
	InputClass::GetInstance()->RegisterKey(VkKeyScan('i'));
	InputClass::GetInstance()->RegisterKey(VkKeyScan('j'));
	InputClass::GetInstance()->RegisterKey(VkKeyScan('k'));
	InputClass::GetInstance()->RegisterKey(VkKeyScan('l'));
	InputClass::GetInstance()->RegisterKey(VkKeyScan(VK_SPACE));

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
		Pitch(-m_moveSpeed);

	}
	if (InputClass::GetInstance()->IsKeyPressed(VkKeyScan('k')))
	{
		Pitch(m_moveSpeed);
	}
	if (InputClass::GetInstance()->IsKeyPressed(VkKeyScan('l')))
	{
		RotateY(m_moveSpeed);
	}
	if (InputClass::GetInstance()->IsKeyPressed(VkKeyScan('j')))
	{
		RotateY(-m_moveSpeed);
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
	float x, y, z, w;
	x = m_cameraPosition.x + p_distance * m_up.x;
	y = m_cameraPosition.y + p_distance * m_up.y;
	z = m_cameraPosition.z + p_distance * m_up.z;
	w = m_cameraPosition.w;

	DirectX::XMFLOAT4 newCameraPos = DirectX::XMFLOAT4(x, y, z, w);

	m_cameraPosition = newCameraPos;
}
void Camera::Pitch(float p_angle)
{
	DirectX::XMMATRIX rotation;
	rotation = DirectX::XMMatrixRotationX(p_angle);

	DirectX::XMStoreFloat4(&m_look, DirectX::XMVector4Transform(DirectX::XMLoadFloat4(&m_look), rotation));
}
void Camera::RotateY(float p_angle)
{
	DirectX::XMMATRIX rotation;
	rotation = DirectX::XMMatrixRotationY(p_angle);

	DirectX::XMStoreFloat4(&m_look, DirectX::XMVector4Transform(DirectX::XMLoadFloat4(&m_look), rotation));
}
void Camera::UpdateMouse()
{
	//if (InputClass::GetInstance()->IsLeftMousePressed())
	//{
	//	m_look.z += m_moveSpeed;
	//}
	//if (InputClass::GetInstance()->IsRightMouseClicked())
	//{
	//	m_look.z -= m_moveSpeed;
	//}
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