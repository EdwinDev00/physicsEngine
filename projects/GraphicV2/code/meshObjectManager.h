
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
	glm::vec3 angularVelocity;
	glm::vec3 angularAcceleration;
	glm::mat3 inertiaTensorInWorld;

	//DEBUG COLOR (Change when collision is detected)
	glm::vec4 debugC = glm::vec4(0, 0, 1, 1);

	GameObject(){};
	GameObject(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, std::string modelPath, std::string texPath = "");

	void Draw(ShaderResource& program, Object::Camera& cam);
	void OnUpdate(float deltaTime)
	{
		//DAMPING
		float linearDamping = 1.0f; // slow down velocity over time
		float angularDamping = 1.0f; // slow down angular velocity over time
		velocity += acceleration * deltaTime; //Gravity 
		position += velocity * deltaTime;
		velocity *= linearDamping;
		acceleration = glm::vec3(0.0f); // reset acceleration for next frame (forces are applied per frame)

		ApplyRotation(angularVelocity * deltaTime);
		angularVelocity *= angularDamping;

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
	inline std::string GetName() { return name; }
	inline const glm::mat4& GetRotationMat() const { return modelRotation; }

	inline const std::vector<Triangles>& GetTriangles() const { return triangles; }

	glm::vec3 CalculateInertiaTensor() const
	{
		glm::vec3 extends = boundingbox.GetExtents();
		float w = extends.x; float h = extends.y; float d = extends.z;

		//Inertia tensor (moment of inertia for cuboid)
		return glm::vec3
		(
			(1.0f / 12.0f) * mass * (h * h + d * d), // Ix
			(1.0f / 12.0f) * mass * (w * w + d * d), // Iy
			(1.0f / 12.0f) * mass * (w * w + h * h)	 // Iz
		);
	}

private:
	glm::mat4 CalculateInertiaTensorMat() const
	{
		glm::vec3 extends = boundingbox.getOriginalExtend();
		float w = extends.x; float h = extends.y; float d = extends.z;

		//Inertia tensor (moment of inertia for cuboid)
		//OPTIMIZE: COMPUTE IN CONSTRUCTOR (PROBLEM: IF RUNTIME SCALE WRONG NEED UPDATE + MASS UPDATE (IMPORTANT!!))
		return 
		glm::inverse(glm::mat4
		(
			(1.0f / 12.0f) * mass * (h * h + d * d),0,0,0, // Ix
			0,(1.0f / 12.0f) * mass * (w * w + d * d),0,0, // Iy
			0,0,(1.0f / 12.0f) * mass * (w * w + h * h),0, // Iz
			0,0,0,1											
		));
	}

	void ApplyRotation(const glm::vec3& angularDisplacement)
	{
		inertiaTensorInWorld = modelRotation * CalculateInertiaTensorMat() * glm::transpose(modelRotation);
		glm::quat orientation(modelRotation);
		glm::quat spin(0,angularDisplacement);
		orientation += spin * orientation * 0.5f;
		orientation = glm::normalize(orientation);
		modelRotation = glm::mat4(orientation);
		transform = modelTranslation * modelRotation * modelScale;
	}
};