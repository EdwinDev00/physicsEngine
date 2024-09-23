#pragma once 

#include <vector>
#include <string>
#include <functional>

#include "core/math/vec2.h"

namespace scene
{
	class Scene 
	{
	public:
		vec2 windowDimension;
		Scene(){}
		virtual ~Scene(){}

		virtual void OnUpdate(float deltaTime){}
		virtual void OnRender(){}
		virtual void OnImGUIRender(){}

		inline void SetDimension(int width, int height)
		{
			this->windowDimension = vec2(width, height);
		}
	};

	class SceneMenu : public Scene
	{
	public:
		//is stack allocated
		SceneMenu(Scene*& currentScenePtr);
		void OnImGUIRender() override;

		template<typename T>
		void RegisterScene(const std::string& sceneName)
		{
			std::cout << "Registering Scene" << sceneName << std::endl;
			//T is the type of class we are creating
			scenes_Inst.push_back(std::make_pair(sceneName, []() { return new T(); }));
		}

		~SceneMenu()
		{
			for (auto& scene : scenes_Inst)
				delete scene.second();
		}

	private:
		Scene*& currentScene; //Tracking of the currently active scene
		//Construct a new scene instance and gets destroys when leaving
		std::vector<std::pair<std::string, std::function<Scene*()>>> scenes_Inst; //List of all the scene instance
	};
}