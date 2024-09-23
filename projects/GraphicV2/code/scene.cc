#include "scene.h"
#include "imgui.h"

namespace scene
{
	SceneMenu::SceneMenu(Scene*& currentScenePtr)
		: currentScene(currentScenePtr)
	{
	}

	void SceneMenu::OnImGUIRender()
	{
		for(auto& scene_v : scenes_Inst)
		{
			if (ImGui::Button(scene_v.first.c_str())) //is pressed, load the scene
				currentScene  = scene_v.second(); //this lambda function returns the new scene pointer
		}
	}

}