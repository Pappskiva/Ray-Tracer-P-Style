#pragma once
#include <DirectXMath.h>
#include <vector>
#include <Windows.h>

#define MOUSE_SPEED 0.00087266f
class Camera
{
public:
	Camera(const Camera&) = delete;
	Camera& operator=(const Camera&) = delete;
	static Camera* GetInstance();

	void Initialize();
	void Update(float p_deltaTime, HWND p_hwnd, int p_screenW, int p_scrrenH);
	void Shutdown();
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
	static Camera* m_instance;
	void Strafe(float p_distance);
	void Walk(float p_distance);
	void UpOrDown(float p_distance);
	void Pitch(float p_angle);
	void RotateY(float p_angle);

	
	void UpdateKeyboard();
	void UpdateMouse(HWND p_hwnd);
	void RotateCamera();

	DirectX::XMFLOAT4 m_cameraPosition;
	DirectX::XMFLOAT4X4 m_projectionMatrix;
	DirectX::XMFLOAT4X4 m_viewMatrix;

	DirectX::XMFLOAT4 m_up;
	DirectX::XMFLOAT4 m_right;
	DirectX::XMFLOAT4 m_look;
	DirectX::XMFLOAT4 m_upDefault;
	DirectX::XMFLOAT4 m_rightDefault;
	DirectX::XMFLOAT4 m_lookDefault;

	float m_moveSpeed;
	float m_rotateSpeed;

	float m_yaw;
	float m_pitch;
	int m_screenWidth;
	int m_screenHeight;
	bool m_rotateUsingMouse;
};

