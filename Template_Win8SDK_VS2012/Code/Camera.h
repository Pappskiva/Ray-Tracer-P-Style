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
	DirectX::XMFLOAT3 GetCameraPos();

private:
	Camera();
	~Camera();
	static Camera* m_instance;

	DirectX::XMFLOAT3 m_cameraPosition;
};

