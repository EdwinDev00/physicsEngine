#pragma once 

#include "scene.h"
#include <memory>
#include "core/math/mat4.h"

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
	private:

		std::shared_ptr<ShaderResource> shader;
		std::shared_ptr<ShaderResource> quadShader;
		std::shared_ptr<ShaderResource> lightShader;
		DirectionalLight* Sun;

		bool RenderDebug = false;
		std::vector<GameObject*> objects; //list of all the object in the current scene
		Object::Camera* cam;

		std::shared_ptr<GBuffer> gBuffer;
		std::shared_ptr<LightBuffer> lightBuffer;

		std::shared_ptr<Quad> quad;
	
		int inspectorSelected = 0; //ImGUI selector
	};
}