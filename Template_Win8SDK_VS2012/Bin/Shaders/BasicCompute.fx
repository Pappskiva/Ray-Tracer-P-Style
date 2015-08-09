//--------------------------------------------------------------------------------------
// BasicCompute.fx
// Direct3D 11 Shader Model 5.0 Demo
// Copyright (c) Stefan Petersson, 2012
//--------------------------------------------------------------------------------------
struct Material
{
	float3 ambient;
	float shininess;
	float3 diffuse;
	float isReflective;
	float3 specular;
	float reflectivefactor;
};
struct TriangleDescription
{
	float point1;
	float point2;
	float point3;
	float normalIndex;

	float TexCoordIndex0;
	float TexCoordIndex1;
	float TexCoordIndex2;
	float padding;

	Material material;
};
struct Ray
{
	float4 m_origin;
	float4 m_direction;
};
struct Sphere
{
	float4 m_position;
	float3 m_color;
	float m_radius;
	Material material;
};
cbuffer perDispatch : register(b0)
{
	float screenWidth;
	float screenHeight;
	int x_dispatchCound;
	int y_dispatchCound;
};
cbuffer everyFrame : register(c0)
{
	float4 cameraPosition;
	float4x4 inverseProjection;
	float4x4 inverseView;
}

//Forward Declare
float4 RaySphereIntersectionTest(float4 p_rayOrigin, float4 p_rayDirection, float3 p_spherePos, float p_sphereRadius, float4 p_color);

Ray CreateRay(uint p_x, uint p_y)
{
	Ray ray;
	ray.m_origin = cameraPosition;

	double normalized_X = ((p_x / screenWidth) - 0.5f) * 2.0f;
	double normalized_Y = (1 - (p_y / screenHeight) - 0.5f) * 2.0f;

	float4 imagePoint = mul(float4(normalized_X, normalized_Y, 1.0f, 1.0f), inverseProjection);
		imagePoint /= imagePoint.w;

	imagePoint = mul(imagePoint, inverseView);

	ray.m_direction = imagePoint - ray.m_origin;
	ray.m_direction = normalize(ray.m_direction);

	return ray;
}
float4 TraceRay(Ray p_ray)
{
	float4 returnColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float sphereRadius = 0.5f;
	Ray ray;
	ray.m_origin = p_ray.m_origin;
	ray.m_direction = p_ray.m_direction;

	returnColor = RaySphereIntersectionTest(ray.m_origin, ray.m_direction, float3(0.0f, 0.0f, 1.0f), sphereRadius, float4(0.0f, 0.0f, 1.0f, 0.0f));
	if (returnColor.x == 0.0f || returnColor.y == 0.0f || returnColor.z == 0.0f){ /*returnColor = float4(1.0f, 1.0f, 1.0f, 1.0f);*/ }
	else{ return returnColor; }

	returnColor = RaySphereIntersectionTest(ray.m_origin, ray.m_direction, float3(0.0f, 1.0f, 1.0f), sphereRadius, float4(0.0f, 1.0f, 1.0f, 0.0f));
	if (returnColor.x == 0.0f || returnColor.y == 0.0f || returnColor.z == 0.0f){ /*returnColor = float4(1.0f, 1.0f, 1.0f, 1.0f); */ }
	else{ return returnColor; }

	returnColor = RaySphereIntersectionTest(ray.m_origin, ray.m_direction, float3(1.0f, 0.0f, 1.0f), sphereRadius, float4(0.0f, 0.0f, 1.0f, 0.0f));
	if (returnColor.x == 0.0f || returnColor.y == 0.0f || returnColor.z == 0.0f){ /*returnColor = float4(1.0f, 1.0f, 1.0f, 1.0f);*/ }
	else{ return returnColor; }

	returnColor = RaySphereIntersectionTest(ray.m_origin, ray.m_direction, float3(1.0f, 1.0f, 1.0f), sphereRadius, float4(0.0f, 0.0f, 1.0f, 0.0f));
	if (returnColor.x == 0.0f || returnColor.y == 0.0f || returnColor.z == 0.0f){ /*returnColor = float4(1.0f, 1.0f, 1.0f, 1.0f); */ }
	else{ return returnColor; }

	returnColor = RaySphereIntersectionTest(ray.m_origin, ray.m_direction, float3(0.0f, 0.0f, -1.0f), sphereRadius, float4(1.0f, 1.0f, 0.0f, 0.0f));
	if (returnColor.x == 0.0f || returnColor.y == 0.0f || returnColor.z == 0.0f){ /*returnColor = float4(1.0f, 1.0f, 1.0f, 1.0f); */ }
	else{ return returnColor; }

	returnColor = RaySphereIntersectionTest(ray.m_origin, ray.m_direction, float3(0.0f, 1.0f, -1.0f), sphereRadius, float4(0.0f, 1.0f, 0.0f, 0.0f));
	if (returnColor.x == 0.0f || returnColor.y == 0.0f || returnColor.z == 0.0f){/* returnColor = float4(1.0f, 1.0f, 1.0f, 1.0f);*/ }
	else{ return returnColor; }

	returnColor = RaySphereIntersectionTest(ray.m_origin, ray.m_direction, float3(0.0f, 0.0f, -1.0f), sphereRadius, float4(1.0f, 0.0f, 0.0f, 0.0f));
	if (returnColor.x == 0.0f || returnColor.y == 0.0f || returnColor.z == 0.0f){ /*returnColor = float4(1.0f, 1.0f, 1.0f, 1.0f);*/ }
	else{ return returnColor; }


	return returnColor;

	//float vtc = 0.0f;
	//
	//float x = p_rayOrigin.x;
	//float y = p_rayOrigin.y;
	//
	//float value = (x*x) + (y*y);
	//vtc = sqrt(value);
	//if (vtc < 1000.0f)
	//{
	//	return float4(0.0f, 1.0f, 0.0f, 0.0f);
	//}
	//else
	//{
	//	return float4(0.0f, 0.0f, 0.0f, 0.0f);
	//}
}

//Intersection Tests
float4 RaySphereIntersectionTest(float4 p_rayOrigin, float4 p_rayDirection, float3 p_spherePos, float p_sphereRadius, float4 p_color)
{
	float4 returnColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 vec = float3(p_spherePos.x - p_rayOrigin.x, p_spherePos.y - p_rayOrigin.y, p_spherePos.z - p_rayOrigin.z);

	float vecLength = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	float powLength = vecLength * vecLength;

	float s2 = ((vec.x * p_rayDirection.x) + (vec.y * p_rayDirection.y) + (vec.z * p_rayDirection.z));

	float powRad = (p_sphereRadius * p_sphereRadius) - ((powLength * powLength)-(s2*s2));

	//if (powRad < 0.0)
	//{
	//}
	//else
	//{
	//	returnColor = p_color;
	//}

	if (s2 >= 0 || powLength <= powRad)
	{
		float m2 = powLength - (s2 * s2);
		if (m2 <= powRad)
		{
			returnColor = p_color;
		}
	}

	//float3 vec = float3(p_spherePos.x - p_rayOrigin.x, p_spherePos.y - p_rayOrigin.y, p_spherePos.z - p_rayOrigin.z);
	//
	//float vecLength = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	//float powLength = vecLength * vecLength;
	//
	//float powRad = p_sphereRadius * p_sphereRadius;
	//
	//float s2 = ((vec.x * p_rayDirection.x) + (vec.y * p_rayDirection.y) + (vec.z * p_rayDirection.z));
	//
	//if (s2 >= 0 || powLength <= powRad)
	//{
	//	float m2 = powLength - (s2 * s2);
	//	if (m2 <= powRad)
	//	{
	//		returnColor = p_color;
	//	}
	//}

	return returnColor;
}

RWTexture2D<float4>						output								: register(u0);
RWStructuredBuffer<float>				temp								: register(u1);

StructuredBuffer<float4>				AllVertex							: register(t0);
StructuredBuffer<TriangleDescription>	AllTriangleDesc						: register(t1);
StructuredBuffer<float3>				ALLNormal							: register(t2);
StructuredBuffer<float2>				AllTexCoord							: register(t3);
Texture2D								BoxTexture							: register(t4);

SamplerState							MeshTexture							: register(s0);

[numthreads(32, 32, 1)]
void main( uint3 threadID : SV_DispatchThreadID )
{
	int2 coord;
	coord.x = threadID.x + 400 * x_dispatchCound;
	coord.y = threadID.y + 400 * y_dispatchCound;
	
	//////////////////////////////////////////////////Primary Ray Stage
	Ray ray;
	ray = CreateRay(coord.x, coord.y);
	//////////////////////////////////////////////////Primary Ray Stage
	//////////////////////////////////////////////////Interaction Stage
	float4 finalColor = TraceRay(ray);

	//////////////////////////////////////////////////Interaction Stage
	//////////////////////////////////////////////////Color Stage
	//float a;
	//a = max(finalColor.x, finalColor.y);
	//a = max(a, finalColor.z);
	//a = max(a, 1.0f);
	//finalColor /= a;
	//int arrayWidth = 1600;
	//temp[coord.x + coord.y * arrayWidth] = finalColor;
	output[threadID.xy] = finalColor;
	//////////////////////////////////////////////////Color Stage
}