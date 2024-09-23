
#pragma once

#include <memory>
#include <algorithm>
#include <unordered_map>
#include <string>

#include "core/math/mat4.h"

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

	void ReadDataV3(const std::string& line, std::vector<vec3>& out);
	void ReadDataV2(const std::string& line, std::vector<vec2>& out);


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
	vec3 position;
	vec3 scale;
	vec3 rotation;

	mat4 modelScale;
	mat4 modelRotation;
	mat4 modelTranslation;

	std::string name;

	std::shared_ptr<Model> modelObject;

public:
	GameObject() {};
	GameObject(vec3 position, vec3 rotation, vec3 scale, std::string modelPath, std::string texPath = "");

	void Draw(ShaderResource& program, Object::Camera& cam);

	inline vec3& GetPosition() { return position; }
	inline std::string GetName() { return name; }
};

//std::shared_ptr<Model> CreateCube(float width, float height, float depth);