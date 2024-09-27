
#pragma once

#include <memory>
#include <algorithm>
#include <unordered_map>
#include <string>

//#include "core/math/mat4.h"
#include "config.h"
#include "dataCollection.h"
#include "Debug.h"

class AABB;
class Model;
struct ObjData;

class ShaderResource;

namespace Object
{
	class Camera;
}

class MeshObject //Model Handler
{
private:

	//singleton
	static MeshObject* instance;
	std::unordered_map<std::string, std::shared_ptr<Model>> modelList;

	std::shared_ptr<Model> LoadObj( std::string modelPath, std::string texPath = "");
	ObjData ParseObj(std::string source, std::string texSource);

	void ReadDataV3(const std::string& line, std::vector<glm::vec3>& out);
	void ReadDataV2(const std::string& line, std::vector<glm::vec2>& out);


public:	
	MeshObject(){}
	MeshObject(const MeshObject&) = delete;
	
	static MeshObject* Get()
	{
		if (instance == nullptr)
			instance = new MeshObject;
		return instance;
	}

	static void Delete()
	{
		delete instance;
		instance = NULL;
	}

	std::shared_ptr<Model> LoadModel(std::string modelPath, std::string texPath ="");
};
	
class GameObject //Graphics node
{
private:
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;

	glm::mat4 modelScale;
	glm::mat4 modelRotation;
	glm::mat4 modelTranslation;
	std::string name;

	std::shared_ptr<Model> modelObject;

	std::vector<Triangles> triangles;
	
	//AABB bounding
public:
	glm::mat4 transform;
	AABB boundingbox;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	float mass;

	GameObject(){};
	GameObject(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, std::string modelPath, std::string texPath = "");

	void Draw(ShaderResource& program, Object::Camera& cam);
	void OnUpdate()
	{
		//Update the AABB bounds according to the changes (translation (position) , scale)
		boundingbox.UpdateBounds(transform);
		Debug::DrawBox(this->boundingbox.GetPosition(), glm::vec3(), this->boundingbox.GetExtents(), glm::vec4(0, 0, 1, 1), 4.0f);
	}

	bool RayMeshIntersection(const Ray& ray, glm::vec3& hitpoint);

	inline glm::vec3& GetPosition() { return position; }
	inline glm::vec3& GetRotation() { return rotation; }
	inline glm::vec3& GetScale() { return scale; }

	inline std::string GetName() { return name; }
};