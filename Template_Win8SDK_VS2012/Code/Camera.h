#pragma once
#include <DirectXMath.h>
#include <vector>
class Camera
{
public:
	Camera(const Camera&) = delete;
	Camera& operator=(const Camera&) = delete;
	static Camera* GetInstance(int p_index);

	void Initialize();
	void Update(float p_deltaTime, int p_index);
	void Shutdown(int p_index);
	DirectX::XMFLOAT4 GetCameraPos();
	DirectX::XMFLOAT4 GetLookAt();

	void SetLens(float p_fovy, float p_aspect, float p_zNear, float p_zFar);
	void UpdateViewMatrix();

	DirectX::XMFLOAT4X4 GetProjectionMatrix();
	DirectX::XMFLOAT4X4 GetViewMatrix();

	void SetPosition(DirectX::XMFLOAT4 p_position);
private:
	Camera();
	~Camera();
	static std::vector<Camera*> m_instance;
	void Strafe(float p_distance);
	void Walk(float p_distance);
	void UpOrDown(float p_distance);
	void Pitch(float p_angle);
	void RotateY(float p_angle);

	
	void UpdateKeyboard();
	void UpdateMouse();

	DirectX::XMFLOAT4 m_cameraPosition;
	DirectX::XMFLOAT4X4 m_projectionMatrix;
	DirectX::XMFLOAT4X4 m_viewMatrix;

	DirectX::XMFLOAT4 m_up;
	DirectX::XMFLOAT4 m_right;
	DirectX::XMFLOAT4 m_look;

	float m_moveSpeed;
};

