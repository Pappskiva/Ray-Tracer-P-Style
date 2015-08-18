#include "Object.h"
#include <iostream>
#include <fstream>

Object* Object::m_object = nullptr;

Object* Object::GetObjectLoader()
{
	if (m_object == nullptr)
	{
		m_object = new Object();
	}
	return m_object;
}
void Object::Shutdown()
{
	delete m_object;
	m_object = nullptr;
}
void Object::LoadObject(char* p_objPath,
	std::vector<DirectX::XMFLOAT4>** p_out_vertices,
	std::vector<DirectX::XMFLOAT2>** p_out_texCoords,
	std::vector<TriangleDescription>** p_out_indices,
	std::vector<DirectX::XMFLOAT3>** p_out_normals)
{
	using namespace std;
	using namespace DirectX;
	bool first = true;

	//Vertex worldposition
	vector<XMFLOAT4>* vertexPosition;
	vertexPosition = new vector < XMFLOAT4 >;
	float x, y, z;

	//Vertex normals
	vector<XMFLOAT3>* vertexNormal;
	vertexNormal = new vector < XMFLOAT3 >;
	float normalX = 0, normalY = 0, normalZ = 0;

	//Vertex texture variables
	vector<XMFLOAT2>* textureCoord;
	textureCoord = new vector < XMFLOAT2 > ;
	float uvx, uvy;

	vector<TriangleDescription>* meshDescription;
	meshDescription = new vector < TriangleDescription > ;
	int		point_index1, point_index2, point_index3,
		normal_index1, normal_index2, normal_index3,
		texCoord_index1, texCoord_index2, texCoord_index3;

	ifstream file;
	file.open(p_objPath);

	char buffer[1024];

	while (file.getline(buffer, 1024))
	{
		char key[20];

		//Texture
		sscanf_s(buffer, "%s ", key, sizeof(key));
		if (key[0] == 't' && key[1] == 'e' && key[2] == 'x')
		{
			sscanf_s(buffer, "v %f %f %f", &x, &y, &z);
			XMFLOAT4 v;
			v.x = x;
			v.y = y;
			v.z = z;
			v.w = 1;
			//le what now?
		}


		//vertex positions
		sscanf_s(buffer, "%s ", key, sizeof(key));
		if (key[0] == 'v' && key[1] != 't' && key[1] != 'n')
		{
			sscanf_s(buffer, "v %f %f %f", &x, &y, &z);
			XMFLOAT4 v;
			//if (first)
			//{
			//	v.x = 1.0f;
			//	v.y = 0.0f;
			//	v.z = 4.0f;
			//	v.w = 1;
			//	first = false;
			//}
			//else
			//{
			v.x = x;
			v.y = y;
			v.z = z;
			v.w = 1;
			//v.x = x / 1000;
			//v.y = y / 1000;
			//v.z = z / 1000;
			//}
			vertexPosition->push_back(v);
		}

		// Vertex normals
		sscanf_s(buffer, "%s ", key, sizeof(key));
		if (key[0] == 'v' && key[1] == 'n')
		{
			sscanf_s(buffer, "vn %f %f %f", &normalX, &normalY, &normalZ);
			XMFLOAT3 vNorm;
			vNorm.x = normalX;
			vNorm.y = normalY;
			vNorm.z = normalZ;
			vertexNormal->push_back(vNorm);
		}

		//Texture Coordinates
		sscanf_s(buffer, "%s ", key, sizeof(key));
		if (key[0] == 'v' && key[1] == 't' )
		{
			sscanf_s(buffer, "vt %f %f", &uvx, &uvy);
			XMFLOAT2 v;
			v.x = uvx;
			v.y = uvy;
			textureCoord->push_back(v);
		}

		//trianlge
		sscanf_s(buffer, "%s ", key, sizeof(key));
		if (key[0] == 'f')
		{
			sscanf_s(buffer, "f %i/%i/%i %i/%i/%i %i/%i/%i",
				&point_index1, &texCoord_index1, &normal_index1,
				&point_index2, &texCoord_index2, &normal_index2,
				&point_index3, &texCoord_index3, &normal_index3);

			TriangleDescription tri;

			tri.Point1 = static_cast<float>(point_index1 - 1);
			tri.Point2 = static_cast<float>(point_index2 - 1);
			tri.Point3 = static_cast<float>(point_index3 - 1);

			tri.TexCoord1 = static_cast<float>(texCoord_index1 - 1);
			tri.TexCoord2 = static_cast<float>(texCoord_index2 - 1);
			tri.TexCoord3 = static_cast<float>(texCoord_index3 - 1);
			tri.PADDING1 = 1.0f;

			tri.NormalIndex = static_cast<float>(normal_index1 - 1);

			float ambient = 0.2f;
			float diffuse = 1.0f;
			float specular = 0.01f;

			tri.material.ambient = XMFLOAT3(ambient, ambient, ambient);
			tri.material.diffuse = XMFLOAT3(diffuse, diffuse, diffuse);
			tri.material.specular = XMFLOAT3(specular, specular, specular);
			tri.material.shininess = 1.0f;
			tri.material.isReflective = 1.0f;
			tri.material.reflectiveFactor = 1.0f;

			meshDescription->push_back(tri);

		}
	}
	file.close();

	//float lowestValue = 0.0f;
	//for (unsigned int i = 0; i < vertexPosition.size(); i++)
	//{
	//	if (lowestValue < vertexPosition[i]->x)
	//	{
	//		lowestValue = vertexPosition[i]->x;
	//	}
	//	if (lowestValue < vertexPosition[i]->y)
	//	{
	//		lowestValue = vertexPosition[i]->y;
	//	}
	//	if (lowestValue < vertexPosition[i]->z)
	//	{
	//		lowestValue = vertexPosition[i]->z;
	//	}
	//}

	*p_out_vertices = vertexPosition;
	*p_out_texCoords = textureCoord;
	*p_out_indices = meshDescription;
	*p_out_normals = vertexNormal;
}