#pragma once
#include <DirectXMath.h>
class Camera
{
public:
	Camera(const Camera&) = delete;
	Camera& operator=(const Camera&) = delete;
	static Camera* GetInstance();

	void Initialize();
	void Update(float p_deltaTime);
	void Shutdown();
	DirectX::XMFLOAT4 GetCameraPos();

	void SetLens(float p_fovy, float p_aspect, float p_zNear, float p_zFar);
	void UpdateViewMatrix();

	DirectX::XMFLOAT4X4 GetProjectionMatrix();
	DirectX::XMFLOAT4X4 GetViewMatrix();

	void SetPosition(DirectX::XMFLOAT4 p_position);
private:
	Camera();
	~Camera();
	static Camera* m_instance;

	DirectX::XMFLOAT4 m_cameraPosition;
	DirectX::XMFLOAT4X4 m_projectionMatrix;
	DirectX::XMFLOAT4X4 m_viewMatrix;

	DirectX::XMFLOAT4 m_up;
	DirectX::XMFLOAT4 m_right;
	DirectX::XMFLOAT4 m_look;
};

