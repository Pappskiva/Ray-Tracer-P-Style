//--------------------------------------------------------------------------------------
// BasicCompute.fx
// Direct3D 11 Shader Model 5.0 Demo
// Copyright (c) Stefan Petersson, 2012
//--------------------------------------------------------------------------------------

struct Ray
{
	float3 m_origin;
	float3 m_direction;
	float m_power;
	float3 m_color;
};

struct Sphere
{
	float3 m_position;
	float m_radius;
};

cbuffer globals : register(b0)
{
	float x;
	float y;
	float z;
	//matrix world;
	//matrix view;
	//matrix projection;
};


StructuredBuffer<float4> inputArray : register(t0);
RWTexture2D<float4> output : register(u0);

[numthreads(32, 32, 1)]
void main( uint3 threadID : SV_DispatchThreadID )
{
	//output[threadID.xy] = float4(float3(0, 0, 1) * (1 - length(threadID.xy - float2(400, 400)) / 400.0f), 1);
	output[threadID.xy] = float4(float3(x, y, z) * (1 - length(threadID.xy - float2(400, 400)) / 400.0f), 1);
}