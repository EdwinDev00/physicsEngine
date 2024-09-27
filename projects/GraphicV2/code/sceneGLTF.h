#pragma once 

#include "scene.h"
#include <memory>

namespace Object
{
	class Camera;
}

class GameObject;
class GBuffer;
class LightBuffer;
class Quad;
class ShaderResource;
class DirectionalLight;

class PhysicsEngine;
class Ray;

namespace scene
{
	class SceneGLTF : public Scene
	{
	public:
		SceneGLTF();
		~SceneGLTF();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGUIRender() override;

		std::shared_ptr<Ray> CreateRayFromMouse(); //return it back to Ray when everything works
		
		//look at the ray direction debug 
		std::shared_ptr<Ray> ray;
		GameObject* hitObject = nullptr; //End refactor it to be local variable

	private:

		std::shared_ptr<ShaderResource> shader;
		std::shared_ptr<ShaderResource> quadShader;
		std::shared_ptr<ShaderResource> lightShader;
		DirectionalLight* Sun;
		
		bool RenderDebug = false;

		PhysicsEngine* phyEngine;
		std::vector<GameObject*> objects; //list of all the object in the current scene
		Object::Camera* cam;

		std::shared_ptr<GBuffer> gBuffer;
		std::shared_ptr<LightBuffer> lightBuffer;

		std::shared_ptr<Quad> quad;
	
		int inspectorSelected = 0; //ImGUI selector

	};
}