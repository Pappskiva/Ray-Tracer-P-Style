#pragma once
#include <string>
#include <vector>
#include "Structures.h"

class Object
{
public:
	static Object* GetObjectLoader();

	void LoadObject(char* p_objPath,
		std::vector<DirectX::XMFLOAT4>** p_out_vertices,
		std::vector<DirectX::XMFLOAT2>** p_out_texCoords,
		std::vector<TriangleDescription>** p_out_indices,
		std::vector<DirectX::XMFLOAT3>** p_out_normals);

	void Shutdown();
private:

	Object(){}
	virtual ~Object(){}
	static Object* m_object;


};