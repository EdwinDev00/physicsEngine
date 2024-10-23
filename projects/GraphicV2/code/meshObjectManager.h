
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

	glm::mat4 transform;
	glm::mat4 modelScale;
	glm::mat4 modelRotation;
	glm::mat4 modelTranslation;
	std::string name;


	std::vector<Triangles> triangles;
	
	//AABB bounding
public:
	std::shared_ptr<Model> modelObject;
	AABB boundingbox;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	float mass;
	glm::vec3 angularVelocity;
	glm::mat3 inertiaTensorInWorld;
	bool isColliding = false;
	std::vector<glm::vec3> colliderVertices; 

	//DEBUG COLOR (Change when collision is detected)
	glm::vec4 debugC = glm::vec4(0, 0, 1, 1);

	GameObject(){};
	GameObject(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, std::string modelPath, bool isStatic = false, std::string texPath = "");

	void Draw(ShaderResource& program, Object::Camera& cam);
	void OnUpdate(float deltaTime)
	{
		velocity += acceleration * deltaTime; //Gravity 
		position += velocity * deltaTime;
		acceleration = glm::vec3(0.0f); // reset acceleration for next frame (forces are applied per frame)
		ApplyRotation(angularVelocity * deltaTime);		
		//Update the AABB bounds according to the changes (translation (position) , scale)
		boundingbox.UpdateBounds(transform);
		Debug::DrawBox(this->boundingbox.GetPosition(), glm::vec3(), this->boundingbox.GetExtents(), debugC, 4.0f);
	}

	bool RayMeshIntersection(const Ray& ray, glm::vec3& hitpoint);

	inline const glm::vec3& GetPosition() const { return position; }
	inline const glm::vec3& GetRotation() const { return rotation; }
	inline const glm::vec3& GetScale() const { return scale; }
	inline  glm::vec3& GetPosition()  { return position; }
	inline  glm::vec3& GetRotation()  { return rotation; }
	inline  glm::vec3& GetScale()  { return scale; }
	inline const std::string& GetName() const { return name; }

	inline const glm::mat4& GetTransform() const { return transform; }
	inline const glm::mat4& GetTranslationMat() const  { return modelTranslation; }
	inline const glm::mat4& GetRotationMat() const { return modelRotation; }
	inline const glm::mat4& GetScaleMat() const { return modelScale; }

	inline const std::vector<Triangles>& GetTriangles() const { return triangles; }

	glm::vec3 TransformLocalPointToWorldSpace(const glm::vec3& localPoint) const
	{
		glm::vec4 worldPoint = modelTranslation * modelRotation * modelScale * glm::vec4(localPoint, 1);
		return glm::vec3(worldPoint);
	}

	glm::vec3 CalculateInertiaTensor() const;

private:
	glm::mat4 CalculateInertiaTensorMat() const;

	void ApplyRotation(const glm::vec3& angularDisplacement);
};