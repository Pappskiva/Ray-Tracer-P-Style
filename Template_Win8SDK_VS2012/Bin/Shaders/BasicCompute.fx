//--------------------------------------------------------------------------------------
// BasicCompute.fx
// Direct3D 11 Shader Model 5.0 Demo
// Copyright (c) Stefan Petersson, 2012
//--------------------------------------------------------------------------------------

struct Ray
{
	float3 m_origin;
	float3 m_direction;
	//float m_power;
	//float3 m_color;
};

struct Sphere
{
	float3 m_position;
	float m_radius;
};

cbuffer perDispatch : register(b0)
{
	//float3 cameraPosition;
	float screenWidth;
	float screenHeight;
	int x_dispatchCound;
	int y_dispatchCound;

	//int dispatchID.y;
	//matrix world;
	//matrix view;
	//matrix projection;
};
cbuffer everyFrame : register(c0)
{
	float4 cameraPosition;
	float4x4 inverseProjection;
	float4x4 inverseView;
}
float4 TraceRay(Ray p_ray)
{
	return float4(0.0f, 1.0f, 255.0f, 0.0f);
}
//Ray CreateRay(uint p_x, uint p_y)
//{
//	//Ray ray;
//	//ray.m_origin = float3(0.0f, 0.0f, 0.0f);
//	//ray.m_direction = float3(0.0f, 0.0f, 0.0f);
//
//	return ray;
//}

RWTexture2D<float4> output : register(u0);

[numthreads(32, 32, 1)]
void main( uint3 threadID : SV_DispatchThreadID )
{
	int2 coord;
	coord.x = threadID.x + 400 * x_dispatchCound;
	coord.y = threadID.y + 400 * y_dispatchCound;

	Ray ray;
	ray.m_origin = float3(0.0f, 0.0f, 0.0f);//Make into cameraposition

	double normalized_X = ((coord.x / screenWidth) - 0.5f) * 2.0f;
	double normalized_Y = (1 - (coord.y / screenHeight) - 0.5f) * 2.0f;
	
	//float4 imagePoint = mul(float4(normalized_X, normalized_Y, 1.0f,1.0f));
	//imagePoint /= imagePoint.w;

	output[threadID.xy] = TraceRay(ray);

	//output[threadID.xy] = float4(float3(threadID.x * 0.001f, 0, 1), 1);
	//output[threadID.xy] = float4(float3(x, y, z) * (1 - length(threadID.xy - float2(400, 400)) / 400.0f), 1);
	//if (dispathID > 0)
	//{
	//	output[threadID.xy] = float4(float3(x, y, -z), 1);
	//}
	//else
	//{
	//	output[threadID.xy] = float4(float3(x, y, z), 1);
	//}
}