#pragma once
#include <memory>
#include <vector>
#include <string>
#include <array>

#include "config.h"

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
	glm::vec3 position;
	glm::vec2 UV;
	glm::vec3 normal;
	glm::vec4 tangent;

	bool operator ==(const Vertex& rhs)
	{
		return position == rhs.position &&
			UV == rhs.UV &&
			normal == rhs.normal;
	}
};

struct PrimitiveData
{
	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> texUVs;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec4> tangents;
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


struct Triangles
{
	std::vector<glm::vec3> vertices;
	glm::vec3 normal;

	Triangles() : normal(0,0,0){}
	Triangles(const std::vector<glm::vec3>& group, glm::vec3& groupNormal) : vertices(group), normal(groupNormal)
	{

	}
};

//ADD RAY CLASS
class Ray
{
public:
	glm::vec3 origin; //beginning of the ray position
	glm::vec3 direction; // ray heading direction (magnitude)

	Ray(){}
	Ray(const glm::vec3& b, const glm::vec3& dir) : origin(b), direction(dir){}
};

//ADD AABB CLASS (attach it to the gameobject)
//MAKE IT APPEAR DEBUG BOX ONTO THE OBJECT 
class AABB 
{
private:
	glm::vec3 originalMinExtent, originalMaxExtent;

public:
	glm::vec3 min; //minimum corner of the AABB
	glm::vec3 max; //maximum corner of the AABB
	glm::vec3 center; //Center of the AABB
	glm::vec3 extend;

	AABB() : min(0,0,0), max(1,1,1), originalMinExtent(0,0,0), originalMaxExtent(1,1,1), center((min + max) / 2.0f), extend (glm::abs(max - min)) {}
	//AABB(const glm::vec3& minC, const glm::vec3& maxC) : min(minC),max(maxC){}

	AABB(const std::shared_ptr<Model>& model)
	{
		//Loop through all the meshes in the model
		float minX = 0, minY = 0, minZ = 0,
			maxX = 0, maxY = 0, maxZ = 0;

		for (auto& mesh : model->meshes)
			for (auto& primitive : mesh.primitives)
				for (auto& vertices : primitive.vertices)
				{
					minX = std::fmin(minX, vertices.position.x);
					minY = std::fmin(minY, vertices.position.y);
					minZ = std::fmin(minZ, vertices.position.z);

					maxX = std::fmax(maxX, vertices.position.x);
					maxY = std::fmax(maxY, vertices.position.y);
					maxZ = std::fmax(maxZ, vertices.position.z);
				}
		min = glm::vec3(minX, minY, minZ);
		max = glm::vec3(maxX, maxY, maxZ);
		originalMinExtent = min;
		originalMaxExtent = max;
		center = (min + max) / 2.0f;
		extend = glm::abs(max - min);
	}

	//returns center of the AABB
	glm::vec3 GetPosition() const { return center; }

	//returns half extents (size of the box)
	glm::vec3 GetExtents() const { return extend; }
	const glm::vec3& getOriginalExtend() const { return originalMaxExtent - originalMinExtent; }
	
	void UpdateBounds(const glm::mat4& transform)
	{		
		glm::vec3 originalCorners[8] = {
			glm::vec3(originalMinExtent.x, originalMinExtent.y, originalMinExtent.z),
			glm::vec3(originalMinExtent.x, originalMinExtent.y, originalMaxExtent.z),
			glm::vec3(originalMinExtent.x, originalMaxExtent.y, originalMinExtent.z),
			glm::vec3(originalMinExtent.x, originalMaxExtent.y, originalMaxExtent.z),
			glm::vec3(originalMaxExtent.x, originalMinExtent.y, originalMinExtent.z),
			glm::vec3(originalMaxExtent.x, originalMinExtent.y, originalMaxExtent.z),
			glm::vec3(originalMaxExtent.x, originalMaxExtent.y, originalMinExtent.z),
			glm::vec3(originalMaxExtent.x, originalMaxExtent.y, originalMaxExtent.z)
		};

		glm::vec3 newMin(FLT_MAX), newMax(-FLT_MAX);

		// Transform all 8 corners and find the new bounds
		for (int i = 0; i < 8; ++i) {
			glm::vec3 transformedCorner = glm::vec3(transform * glm::vec4(originalCorners[i], 1.0f));
			newMin = glm::min(newMin, transformedCorner);
			newMax = glm::max(newMax, transformedCorner);
		}

		min = newMin;
		max = newMax;
		center = (max + min) / 2.0f;
		extend = glm::abs(max - min);
	}

	//Function for checking intersection with another AABB
	bool Intersects(const AABB& other) const
	{
		return (min.x <= other.max.x && max.x >= other.min.x) &&
			   (min.y <= other.max.y && max.y >= other.min.y) &&
			   (min.z <= other.max.z && max.z >= other.min.z);
	}
};

struct Face //polytope face
{
	glm::vec3 normal;
	float distance = FLT_MAX;
	std::array<int, 3> polytopeIndices = {}; //indices of the points in the polytope that forms this face

	/*Face() : normal(glm::vec3(0)), distance(FLT_MAX), polytopeIndices({}) {}
	Face(const glm::vec3& normal, float& distance, std::array<int,3> indices) : normal(normal), distance(distance),polytopeIndices(indices){}*/
};