#pragma once
#include <memory>
#include <vector>
#include <string>

#include "core/math/mat4.h"

#include "texture.h"
#include "vertexArray.h"
#include "indexBuffer.h"

class VertexBuffer;
class IndexBuffer;
class VertexArray;

namespace material
{
	class BaseMaterial;
}

struct Vertex
{
	vec3 position;
	vec2 UV;
	vec3 normal;
	vec4 tangent;

	bool operator ==(const Vertex& rhs)
	{
		return position == rhs.position &&
			UV == rhs.UV &&
			normal == rhs.normal;
	}
};

struct PrimitiveData
{
	std::vector<vec3> positions;
	std::vector<vec2> texUVs;
	std::vector<vec3> normals;
	std::vector<vec4> tangents;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	material::BaseMaterial* baseMat;

	virtual std::vector<Vertex> ConstructMeshData() = 0;
};

struct GLTFData : public PrimitiveData
{
	GLTFData(){}
	std::vector<Vertex> ConstructMeshData() override;
};

struct ObjData : public PrimitiveData
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> Vertexindices;
	std::vector<unsigned int> Textureindices;
	std::vector<unsigned int> Normalindices;

	std::vector<Vertex> ConstructMeshData() override;
};

struct Primitive
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	VertexArray vao;
	VertexBuffer vbo;
	IndexBuffer ibo;
	material::BaseMaterial* baseMaterial; 

	Primitive(PrimitiveData* data);
	
	void Upload();
	void Clear();
};
//
struct Mesh
{
	std::vector<Primitive> primitives;
	Mesh(){}

	Mesh(PrimitiveData* primData) 
	{
		primitives.push_back(Primitive(primData)); 
	}

	Mesh(std::vector<PrimitiveData*>& prim) 
	{
		for(auto& data : prim)
			primitives.push_back(Primitive(data));
	}
};

class Model //Graphic Node
{
private:
	void UploadToGPU();

public:
	std::vector<Mesh> meshes;
	Model() {}
	
	Model(Mesh mesh) {
		meshes.push_back(mesh);
		UploadToGPU();
	}
	Model(std::vector<Mesh> meshList) {
		for(auto& mesh : meshList)
		{
			meshes.push_back(mesh);
			UploadToGPU();
		}
	}

	void SimpleDraw();
};
