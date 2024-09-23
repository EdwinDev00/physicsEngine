#pragma once

#include <vector>
#include <string>

#include "core/math/vec3.h"
#include "core/math/vec4.h"
#include "core/math/mat4.h"

#include "shaderResource.h"
#include "Debug.h"

namespace Object
{
	class Camera;
}

namespace scene
{
	class SceneObjLight;
	class SceneGLTF;
}

class Model;

class BaseLight
{

public:
	BaseLight(){};
	virtual ~BaseLight(){};
	virtual void Update(ShaderResource& program, Object::Camera& cam){};
	virtual void DisableLight(ShaderResource& program){};

};


class PointLight : public BaseLight 
{
private:
	friend class scene::SceneGLTF; 

	vec3 position;
	vec3 color; //Default white
	float intensity;
	float intensityMax; //light max

	float constant = 1.0f;
	float linear = 1.0f;
	float quadratic = 2.5f;
	float radius; 

	int lightID; //individual need to have its unique id
	bool bOrbit = false;

	std::string lightString;
	std::string Spos;
	std::string Scolor;
	std::string Sintensity;
	std::string Sconstant;
	std::string Slinear;
	std::string Squadratic;

	std::shared_ptr<Model> sphereModel;

	void Orbit(float totalTime);
public:
	PointLight(){};
	PointLight(vec3 pos, vec3 color, float intensity);
	~PointLight() { 
		
	};

	void Update(ShaderResource& program, Object::Camera& cam) override; //Update the light source;
	void DisableLight(ShaderResource& program) override; //Disable the light source;

	inline vec3 GetPos() { return position; }
	inline float GetRadius() { return radius; }

};


class DirectionalLight : public BaseLight //Represent the light
{
private:
	friend class scene::SceneGLTF;

	vec3 direction;
	vec3 color; //Default white
	float intensity = 0.2f;

	int lightID; //individual need to have its unique id

public:
	DirectionalLight() {};
	DirectionalLight(vec3 dir, vec3 color, float intensity);
	~DirectionalLight() {};

	void Update(ShaderResource& program, Object::Camera& cam) override; //Update the light source;
	void DisableLight(ShaderResource& program) override; //Disable the light source;

	inline vec3 GetDirection() { return direction; }
};


class LightManager
{
private:
	static LightManager* instance;
	static int lightCount;
	std::vector<BaseLight*> lights; 

	const int MaximumLights = 128;
public:
	std::vector<PointLight*> pLights;

	LightManager() {}
	~LightManager()
	{
	}
	inline static LightManager* Get() 
	{
		if (instance == nullptr)
			instance = new LightManager;
		return instance;
	}

	void PointUpdate(ShaderResource& program, Object::Camera& cam)
	{
		for (int i = 0; i < pLights.size(); i++)
		{
			pLights[i]->Update(program, cam);
			Debug::DrawSphere(pLights[i]->GetPos(), pLights[i]->GetRadius(), vec4(0, 1, 0, 1));
		}
	};


	void Clear()
	{
		for (auto& light : lights)
			delete light;
		lights.clear();
		lightCount = 0;
		pLights.clear();
	}; //Delete all the lights inside the vector

	inline void RegisterLight(BaseLight* light) 
	{ 
		lights.push_back(light); 
		lightCount++; 
	};

	inline static int GetLightCount() { return lightCount; }

	inline PointLight* GetLight(int i)
	{
		return ((PointLight* )lights[i]);
	}
};
