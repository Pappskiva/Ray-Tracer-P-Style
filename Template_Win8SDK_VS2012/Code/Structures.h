
#include <DirectXMath.h>
#define LIGHT_COUNT 10
#define SPHERE_COUNT 3

struct DispatchBufferStruct
{
	float screenWidth;
	float screenHeight;
	int x_dispatchCound;
	int y_dispatchCound;
};
struct EveryFrameStruct
{
	DirectX::XMFLOAT4 cameraPosition;
	DirectX::XMFLOAT4X4 inverseProjection;
	DirectX::XMFLOAT4X4 inverseView;
	int Padding0;
	int Padding1;
	int Padding2;
	unsigned int Padding3;
};
struct Material
{
	DirectX::XMFLOAT3 ambient;
	float shininess;
	DirectX::XMFLOAT3 diffuse;
	float isReflective;
	DirectX::XMFLOAT3 specular;
	float reflectiveFactor;
};
struct SphereStruct
{
	DirectX::XMFLOAT4 center;
	DirectX::XMFLOAT3 color;
	float radius;
	Material material;
};
struct TriangleDescription
{
	float Point1;
	float Point2;
	float Point3;
	float NormalIndex;
	float TexCoord1;
	float TexCoord2;
	float TexCoord3;
	float PADDING1;

	Material material;
};

struct Primitive
{
	SphereStruct Sphere[SPHERE_COUNT];
};


struct PointLightData
{
	DirectX::XMFLOAT4 position;
	DirectX::XMFLOAT4 color;
};

struct LightBuffer
{
	PointLightData pointLight[LIGHT_COUNT];
};

enum LightMovement
{
	MoveXPos,
	MoveXNeg,
	MoveZPos,
	MoveZNeg
};