//--------------------------------------------------------------------------------------
// BasicCompute.fx
// Direct3D 11 Shader Model 5.0 Demo
// Copyright (c) Stefan Petersson, 2012
//--------------------------------------------------------------------------------------
#pragma pack_matrix(row_major)
static const float EPSILON = 0.0001f;

static const uint PRIMITIVE_INDICATOR_NONE = 0;
static const uint PRIMITIVE_INDICATOR_SPHERE = 1;
static const uint PRIMITIVE_INDICATOR_TRIANGLE = 2;

static const uint NUMBER_OF_LIGHTS = 10;
static const uint NUMBER_OF_SPHERES = 10;
static const float4 BLACK = float4(0.0f, 0.0f, 0.0f, 0.0f);
static const float4 WHITE = float4(1.0f, 1.0f, 1.0f, 0.0f);
static const float4 BLUE = float4(0.0f, 0.0f, 1.0f, 0.0f);
static const float4 RED = float4(1.0f, 0.0f, 0.0f, 0.0f);
static const float4 GREEN = float4(0.0f, 1.0f, 0.0f, 0.0f);

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
	float point0;
	float point1;
	float point2;
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
struct PointLightStruct
{
	float4 position;
	float4 color;
};
cbuffer everyFrame		: register(b0)
{
	float4 cameraPosition;
	float4x4 inverseProjection;
	float4x4 inverseView;
	int Padding0;
	int Padding1;
	int Padding2;
	uint numberOfBounces;
}
cbuffer SphereBuffer : register(b1)
{
	Sphere sphere[NUMBER_OF_SPHERES];
}
cbuffer LightBuffer		: register(b2)
{
	PointLightStruct pointLight[NUMBER_OF_LIGHTS];
}
cbuffer perDispatch		: register(b3)
{
	float screenWidth;
	float screenHeight;
	int x_dispatchCound;
	int y_dispatchCound;
};
RWTexture2D<float4>						output								: register(u0);

StructuredBuffer<float4>				AllVertex							: register(t0);
StructuredBuffer<TriangleDescription>	AllTriangleDesc						: register(t1);
StructuredBuffer<float3>				AllNormal							: register(t2);
StructuredBuffer<float2>				AllTexCoord							: register(t3);
Texture2D								BoxTexture							: register(t4);

SamplerState							MeshTextureSampler					: register(s0);

//Forward Declare
Ray CreateRay(uint p_x, uint p_y);
float RaySphereIntersectionTest(in Ray p_ray, in Sphere p_sphere);
float RayTriangleIntersectionTest(in Ray p_ray, in uint p_index);
Ray RayJump(inout Ray p_ray,out float4 p_out_collideNormal, out Material p_out_material, out uint p_out_primitiveIndex, out uint p_out_primitiveType);
void GetClosestPrimitive(in Ray p_ray, in bool p_isSphereIntersection, in uint p_amount, out uint p_hitPrimitive, out uint p_closestPrimitiveIndex, out float p_distanceToClosestPrimitive);
float4 ShadeCalculation(in Ray p_ray, in uint p_primitiveIndex, in uint p_primitiveType, in float4 p_collideNormal, in Material p_material);
float4 GetPrimitiveColor(in uint p_primitiveIndex, in uint p_primitiveType, in float3 p_intersectPos);
float4 GetTriangleTexture(in uint p_primitiveIndex, in float3 p_intersectPos);
float2 GetTriangleTextureCoordinates(in uint p_primitiveIndex, in float3 p_intersectPos);
float GetTriangleArea(float3 p_point0, float3 p_point1, float3 p_point2);
float4 CalculateLight(Material p_material, float4 p_hitPosition, float4 p_surfaceNormal, PointLightStruct p_lightData);
float4 CalculatPhongLighting(Material M, float4 L, float4 N, float4 R, float4 V);
bool InLight(in Ray p_ray, in uint p_primitiveIndex, in uint p_primitiveType, in uint p_lightIndex);
float GetReflective(in uint p_primitiveIndex, in uint p_primitiveType);
float GetReflectiveFactor(in uint p_primitiveIndex, in uint p_primitiveType);



Ray CreateRay(uint p_x, uint p_y)
{
	Ray ray;
	ray.m_origin = cameraPosition;

	double normalized_x = ((p_x / screenWidth) - 0.5) * 2.0;					// HARDCODED SCREENSIZE
	double normalized_y = (1 - (p_y / screenHeight) - 0.5) * 2.0;				// HARDCODED SCREENSIZE

	float4 imagePoint = mul(float4(normalized_x, normalized_y, 1.0f, 1.0f), inverseProjection);
		imagePoint /= imagePoint.w;

	imagePoint = mul(imagePoint, inverseView);

	ray.m_direction = imagePoint - ray.m_origin;
	ray.m_direction = normalize(ray.m_direction);

	return ray;
}
float4 TraceRay(Ray p_ray)
{
	float4 returnColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	Ray nextRay = p_ray;

	float4 collideNormal;
	float4 intersectionPoint;
	Material material;
	uint primitiveIndex;
	uint primitiveType;

	nextRay = RayJump(nextRay, collideNormal, material, primitiveIndex, primitiveType);

	if (primitiveType != PRIMITIVE_INDICATOR_NONE)//If it hits something
	{
		returnColor = ShadeCalculation(nextRay, primitiveIndex, primitiveType, collideNormal, material);
	}

	uint isReflective;
	float reflectiveFactor = 1.0f;

	for (uint i = 0; i < numberOfBounces - 1; i++)
	{
		isReflective = GetReflective(primitiveIndex, primitiveType);
		if (isReflective == 1)//If reflective
		{
			reflectiveFactor *= GetReflectiveFactor(primitiveIndex, primitiveType);
			if (reflectiveFactor > -EPSILON && reflectiveFactor < EPSILON)
			{
				break;
			}
			nextRay = RayJump(nextRay, collideNormal, material, primitiveIndex, primitiveType);

			if (primitiveType != PRIMITIVE_INDICATOR_NONE)
			{
				returnColor += reflectiveFactor * ShadeCalculation(nextRay, primitiveIndex, primitiveType, collideNormal, material);
			}
			else if (primitiveType == PRIMITIVE_INDICATOR_NONE)
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
	return returnColor;
}
Ray RayJump(inout Ray p_ray, out float4 p_out_collideNormal, out Material p_out_material, out uint p_out_primitiveIndex, out uint p_out_primitiveType)
{

	p_out_primitiveIndex = 0;
	p_out_primitiveType = 0;
	p_out_collideNormal = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 collidePos;
	uint sphereIndex = 0;
	uint triangleIndex = 0;
	float distanceToClosestSphere = 0.0f;
	float distanceToClosestTriangle = 0.0f;
	uint sphereHit, triangleHit;

	uint triangle_amount;
	AllTriangleDesc.GetDimensions(triangle_amount, sphereIndex);

	GetClosestPrimitive(p_ray, true, NUMBER_OF_SPHERES, sphereHit, sphereIndex, distanceToClosestSphere);
	GetClosestPrimitive(p_ray, false, triangle_amount, triangleHit, triangleIndex, distanceToClosestTriangle);

	//If there are any hits att all
	if (distanceToClosestTriangle == 0.0f && distanceToClosestSphere == 0.0f)
	{
		p_out_primitiveType = PRIMITIVE_INDICATOR_NONE;
		return p_ray;
	}

	const float VERY_SMAL_PADDING_NUMBER = 0.001f;
	//////////////////////////Checks Which primitive is closest
	if ((sphereHit != -1 && triangleHit != -1 && distanceToClosestSphere < distanceToClosestTriangle) || sphereHit != -1 && triangleHit == -1)
	{
		collidePos = p_ray.m_origin + (distanceToClosestSphere - VERY_SMAL_PADDING_NUMBER) * p_ray.m_direction;

		p_out_collideNormal = normalize(collidePos - sphere[sphereIndex].m_position);
		p_out_material = sphere[sphereIndex].material;
		p_out_primitiveIndex = sphereIndex;
		p_out_primitiveType = PRIMITIVE_INDICATOR_SPHERE;

		p_ray.m_origin = collidePos;
		p_ray.m_direction = float4(reflect(p_ray.m_direction.xyz, p_out_collideNormal.xyz), 0.0f);
	}
	else if ((sphereHit != -1 && triangleHit != -1 && distanceToClosestTriangle < distanceToClosestSphere) || sphereHit == -1 && triangleHit != -1)
	{
		collidePos = p_ray.m_origin + (distanceToClosestTriangle - VERY_SMAL_PADDING_NUMBER)* p_ray.m_direction;

		float3 normalized;
		normalized = normalize(AllNormal[(uint)AllTriangleDesc[triangleIndex].normalIndex]);
		p_out_collideNormal = float4(normalized, 0.0f);
		p_out_material = AllTriangleDesc[triangleIndex].material;
		p_out_primitiveIndex = triangleIndex;
		p_out_primitiveType = PRIMITIVE_INDICATOR_TRIANGLE;

		p_ray.m_origin = collidePos;
		p_ray.m_direction = float4(reflect(p_ray.m_direction.xyz, -p_out_collideNormal.xyz), 0.0f);
	}
	else
	{
		p_out_primitiveType = PRIMITIVE_INDICATOR_NONE;
	}

	//////////////////////////////////////////////////////////
	//p_out_material.ambient = float3(0.0f, 0.0f, 0.0f);
	//p_out_material.shininess = 0.0f;
	//p_out_material.diffuse = float3(0.0f, 0.0f, 0.0f);
	//p_out_material.isReflective = 0.0f;
	//p_out_material.specular = float3(0.0f, 0.0f, 0.0f);
	//p_out_material.reflectivefactor = 0.0f;
	//p_out_collideNormal = float4(0.0f, 0.25f, 0.25f, 0.0f);
	//p_out_primitiveIndex = 0;
	//p_out_primitiveType = 0;

	return p_ray;

}
void GetClosestPrimitive(in Ray p_ray, in bool p_isSphereIntersection, in uint p_amount, out uint p_hitPrimitive, out uint p_closestPrimitiveIndex, out float p_distanceToClosestPrimitive)
{
	p_hitPrimitive = -1;
	float temp = 0.0f;
	p_distanceToClosestPrimitive = 0.0f;

	for (uint i = 0; i < p_amount; i++)
	{
		if (p_isSphereIntersection == true)
		{
			temp = RaySphereIntersectionTest(p_ray, sphere[i]);
		}
		else
		{
			temp = RayTriangleIntersectionTest(p_ray, i);
		}

		if (temp != 0.0f && temp > 0.0f)
		{
			p_hitPrimitive = 1;
			if (temp < p_distanceToClosestPrimitive || p_distanceToClosestPrimitive == 0.0f)
			{
				p_distanceToClosestPrimitive = temp;
				p_closestPrimitiveIndex = i;
			}
		}
	}

}
//Intersection Tests
float RaySphereIntersectionTest(in Ray p_ray, in Sphere p_sphere)
{
	if (p_sphere.m_radius < 1.0f)
	{
		return 0.0f;
	}
	float4 distance = p_ray.m_origin - p_sphere.m_position;
	float a, b, t, t1, t2;

	b = dot(p_ray.m_direction, distance);
	a = dot(distance, distance) - (p_sphere.m_radius * p_sphere.m_radius);
	float temp = b*b - a;
	if (temp >= 0)
	{
		t = sqrt(temp);
		t1 = -b + t;
		t2 = -b - t;
		if (t1 > 0.0f || t2 > 0.0f)
		{
			if (t1 < t2 && t1 > 0)
			{
				return t1;
			}
			else if (t2 > 0)
			{
				return t2;
			}
		}

	}
	return 0.0f; // if didn't hit
}
float RayTriangleIntersectionTest(in Ray p_ray, in uint p_index)
{
	float3 e1, e2;
	float det, inv_det, u, v, t;

	float Point0, Point1, Point2;

	uint tempIndex = p_index;

	Point0 = AllTriangleDesc[tempIndex].point0;
	Point1 = AllTriangleDesc[tempIndex].point1;
	Point2 = AllTriangleDesc[tempIndex].point2;

	e1 = AllVertex[(uint)Point1].xyz - AllVertex[(uint)Point0].xyz;
	e2 = AllVertex[(uint)Point2].xyz - AllVertex[(uint)Point0].xyz;

	float3 p = cross(p_ray.m_direction.xyz, e2);

	det = dot(e1, p);

	if (det > -EPSILON && det < EPSILON)
	{
		return 0.0f;
	}

	inv_det = 1.0f / det;

	float3 T = p_ray.m_origin.xyz - AllVertex[Point0].xyz;

	u = dot(T, p)*inv_det;

	if (u < 0.0f || u > 1.0f)
	{
		return 0.0f;
	}

	float3 Q = cross(T, e1);

	v = dot(p_ray.m_direction.xyz, Q) * inv_det;

	if (v < 0.0f || u + v > 1.0f)
	{
		return 0.0f;
	}

	t = dot(e2, Q) * inv_det; 

	if (t > EPSILON)
		return t;

	return 0.0f; // if didn't hit
}
//Shade and light calculations
float4 ShadeCalculation(in Ray p_ray, in uint p_primitiveIndex, in uint p_primitiveType, in float4 p_collideNormal, in Material p_material)
{
	float4 illumination = float4(0.0f, 0.0f, 0.0f, 0.0f);

	for (unsigned int i = 0; i < NUMBER_OF_LIGHTS; i++)
	{
		if (pointLight[i].color.x != 0.0f && pointLight[i].color.y != 0.0f && pointLight[i].color.z != 0.0f)
		{
			bool isLitByLight = InLight(p_ray, p_primitiveIndex, p_primitiveType, i);
			if (isLitByLight == true)
			{
				illumination += CalculateLight(p_material, p_ray.m_origin, p_collideNormal, pointLight[i]) * pointLight[i].color;
			}
		}
	}
	illumination += float4(p_material.ambient, 1.0f);
	illumination *= GetPrimitiveColor(p_primitiveIndex, p_primitiveType, p_ray.m_origin.xyz);
	return illumination;
}
bool InLight(in Ray p_ray, in uint p_primitiveIndex, in uint p_primitiveType, in uint p_lightIndex)
{
	uint closestSphereIndex, closestTriangleIndex;
	float distanceToClosestSphere, distanceToClosestTriangle;
	uint sphereHit, triangleHit;

	Ray towardsLightSource;
	towardsLightSource.m_origin = pointLight[p_lightIndex].position;
	towardsLightSource.m_direction = normalize(p_ray.m_origin - pointLight[p_lightIndex].position);

	uint triangle_amount;
	AllTriangleDesc.GetDimensions(triangle_amount, closestSphereIndex);

	GetClosestPrimitive(towardsLightSource, true, NUMBER_OF_SPHERES, sphereHit, closestSphereIndex, distanceToClosestSphere);
	GetClosestPrimitive(towardsLightSource, false, triangle_amount, triangleHit, closestTriangleIndex, distanceToClosestTriangle);

	if (sphereHit != -1 && triangleHit != -1)
	{
		if (p_primitiveType == PRIMITIVE_INDICATOR_TRIANGLE)
		{
			if (distanceToClosestTriangle < distanceToClosestSphere)
			{
				if (p_primitiveIndex == closestTriangleIndex)
				{
					return true;
				}
			}
		}
		else if (p_primitiveType == PRIMITIVE_INDICATOR_SPHERE)
		{
			if (distanceToClosestTriangle > distanceToClosestSphere)
			{
				if (p_primitiveIndex == closestSphereIndex)
				{
					return true;
				}
			}
		}
	}
	else if (sphereHit != -1 && triangleHit == -1)
	{
		if (p_primitiveIndex == closestSphereIndex)
		{
			return true;
		}
	}
	else if (sphereHit == -1 && triangleHit != -1)
	{
		if (p_primitiveIndex == closestTriangleIndex)
		{
			return true;
		}
	}

	return false;
}
float4 CalculateLight(Material p_material, float4 p_hitPosition, float4 p_surfaceNormal, PointLightStruct p_lightData)
{
	float4 L = p_lightData.position - p_hitPosition; //vector from the point on the surface to each light
	float d = length(L);

	float r = 5000.0f;
	float a = 1.0f/(r * r);
	float b = 2.0f / r;
	float c = 1.0f;

	//d = distance between the light and the surface being shaded
	//a = quadratic attenuation factor
	//b = linear attenuation factor
	//c = constant attenuation factor
	float lightAttenuation = 1 / (a*d*d + b*d + c);
	L = normalize(L);

	float4 N = normalize(p_surfaceNormal);
	float4 R = normalize(2 * saturate(dot(L, N)) * N - L);//direction a reflected ray of light would take from this point on the surface
	float4 V = normalize(cameraPosition - p_hitPosition);// direction pointing towards the viewer

	return lightAttenuation * CalculatPhongLighting(p_material, L, N, R, V);
}
float4 CalculatPhongLighting(Material M, float4 L, float4 N, float4 R, float4 V)
{//soft specular calculations
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = saturate(dot(L, N));
	if (diffuse.x > 0.0f)
	{
		specular = float4(M.specular, 1.0f) * pow(saturate(dot(R, V)), M.shininess);
	}
	diffuse *= float4(M.diffuse, 1.0f);

	return (diffuse + specular);
}
//Get color from texture
float4 GetPrimitiveColor(in uint p_primitiveIndex, in uint p_primitiveType, in float3 p_intersectPos)
{
	if (p_primitiveType == PRIMITIVE_INDICATOR_SPHERE)
	{
		return float4(sphere[p_primitiveIndex].m_color, 0.0f);
	}
	else if (p_primitiveType == PRIMITIVE_INDICATOR_TRIANGLE)
	{
		float4 colorToAdd = float4(1.0f, 1.0f, 1.0f, 1.0f);
		return GetTriangleTexture(p_primitiveIndex, p_intersectPos) * colorToAdd;
	}
	return float4(0.0f,0.0f,0.0f,0.0f);//Return black 
}
float4 GetTriangleTexture(in uint p_primitiveIndex, in float3 p_intersectPos)
{
	float2 uv = GetTriangleTextureCoordinates(p_primitiveIndex, p_intersectPos);

	return BoxTexture.SampleLevel(MeshTextureSampler, uv, 0);
}
float2 GetTriangleTextureCoordinates(in uint p_primitiveIndex, in float3 p_intersectPos)
{
	TriangleDescription triangleDescription = AllTriangleDesc[p_primitiveIndex];
	float totalArea = GetTriangleArea(AllVertex[triangleDescription.point0].xyz, AllVertex[triangleDescription.point1].xyz, AllVertex[triangleDescription.point2].xyz);

	float area0, area1, area2;
	area0 = GetTriangleArea(AllVertex[triangleDescription.point1].xyz, AllVertex[triangleDescription.point2].xyz, p_intersectPos);
	area1 = GetTriangleArea(AllVertex[triangleDescription.point0].xyz, AllVertex[triangleDescription.point2].xyz, p_intersectPos);
	area2 = GetTriangleArea(AllVertex[triangleDescription.point0].xyz, AllVertex[triangleDescription.point1].xyz, p_intersectPos);

	float b0, b1, b2;
	b0 = area0 / totalArea;
	b1 = area1 / totalArea;
	b2 = area2 / totalArea;

	float2 texcoord0 = AllTexCoord[triangleDescription.TexCoordIndex0];
	float2 texcoord1 = AllTexCoord[triangleDescription.TexCoordIndex1];
	float2 texcoord2 = AllTexCoord[triangleDescription.TexCoordIndex2];

	return b0 * texcoord0 + b1 * texcoord1 + b2 * texcoord2 * triangleDescription.padding;/////////////////////////////////////////////////////////PADDING????????TODO
}
float GetTriangleArea(float3 p_point0, float3 p_point1, float3 p_point2)
{
	float border0, border1, border2;
	border0 = length(p_point0 - p_point1);
	border1 = length(p_point0 - p_point2);
	border2 = length(p_point1 - p_point2);
	
	//float temp1, temp2;
	//if ((temp1 = border0) == (temp2 = border1) || (temp1 = border0) == (temp2 = border2) || (temp1 = border1) == (temp2 = border2))
	//{
	//	return temp1 * temp2 / 2;
	//}

	float s = 0.5f * (border0 + border1 + border2);
	float area = sqrt(s * (s - border0) * (s - border1) * (s - border2));
	return area;
}
//Get reflective factor
float GetReflective(in uint p_primitiveIndex, in uint p_primitiveType)
{
	if (p_primitiveType == PRIMITIVE_INDICATOR_SPHERE)
		return sphere[p_primitiveIndex].material.isReflective;
	else if (p_primitiveType == PRIMITIVE_INDICATOR_TRIANGLE)
		return AllTriangleDesc[p_primitiveIndex].material.isReflective;
	return 0.0f;
}
float GetReflectiveFactor(in uint p_primitiveIndex, in uint p_primitiveType)
{
	if (p_primitiveType == PRIMITIVE_INDICATOR_SPHERE)
		return sphere[p_primitiveIndex].material.reflectivefactor;
	else if (p_primitiveType == PRIMITIVE_INDICATOR_TRIANGLE)
		return AllTriangleDesc[p_primitiveIndex].material.reflectivefactor;
	return 0.0f;
}

[numthreads(32, 32, 1)]
void main( uint3 threadID : SV_DispatchThreadID )
{
	int2 coord;
	coord.x = threadID.x + screenWidth /2 * x_dispatchCound;
	coord.y = threadID.y + screenHeight /2 * y_dispatchCound;
	//////////////////////////////////////////////////Primary Ray Stage
	Ray ray;
	ray = CreateRay(coord.x, coord.y);
	//////////////////////////////////////////////////Primary Ray Stage
	//////////////////////////////////////////////////Interaction Stage
	float4 finalColor = BLACK;
	finalColor = TraceRay(ray);
	//////////////////////////////////////////////////Interaction Stage
	//////////////////////////////////////////////////Color Stage
	float a;
	a = max(finalColor.x, finalColor.y);
	a = max(a, finalColor.z);
	a = max(a, 1.0f);
	finalColor /= a;

	output[coord] = finalColor;
	//////////////////////////////////////////////////Color Stage
}