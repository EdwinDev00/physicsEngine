#include "sceneGLTF.h"
#include "inputHandler.h"

#include "imgui.h"
#include "gl/glew.h"
#include "GLFW/glfw3.h"


#include "camera.h"
#include "light.h"

#include "meshObjectManager.h"
#include "dataCollection.h"
#include "gBuffer.h"

#include "Debug.h"

namespace scene
{
	scene::SceneGLTF::SceneGLTF()
	{
		//Projection matrix ratio: 16:9 //Model view proj matrix

		cam = new Object::Camera();
		cam->Setpos(glm::vec3(0, 0, 0));
		cam->SetPerspective(30, 1920, 1080);
		cam->SetView(glm::lookAt(cam->GetPosition(), glm::vec3(0, 0, 0), cam->GetUP()));

		SetDimension(1920, 1080);
		gBuffer = std::make_shared<GBuffer>(windowDimension.x, windowDimension.y);
		lightBuffer = std::make_shared<LightBuffer>(windowDimension.x, windowDimension.y);

		
		quad = std::make_shared<Quad>();

		//Obj
		GameObject* plane = new GameObject(glm::vec3(0, -.25f, 1), glm::vec3(0, 0, 0), glm::vec3(6, 6, 6), "../projects/GraphicV2/asset/plane.obj", "../projects/GraphicV2/texture/tile.png");
		//GLTF
		GameObject* CubeGLTF = new GameObject(glm::vec3(2, 1, -2), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), "../projects/GraphicV2/asset/Cube/Cube.gltf");
		objects.push_back(plane);
		objects.push_back(CubeGLTF);

		shader = std::make_shared<ShaderResource>("../projects/GraphicV2/code/gBuffer.glsl");
		quadShader = std::make_shared<ShaderResource>("../projects/GraphicV2/code/quad.glsl");
		lightShader = std::make_shared<ShaderResource>("../projects/GraphicV2/code/LightPass.glsl");


		for(int i = 0; i < 2; i++)
		{
			float rnd = (rand() % 5) / 5.0f;
			PointLight* pLight1 = new PointLight(glm::vec3(0, 3, 0), glm::vec3(rnd, 1, rnd), 1);
		}
 
		//Directional light (sun)
		Sun = new DirectionalLight(glm::vec3(0, -1, 0), glm::vec3(.5f, .5f, .5f), 0.2f);

		Debug::Init();
	}

	scene::SceneGLTF::~SceneGLTF()
	{	
		//Deallocate the memory
		LightManager::Get()->Clear();
		delete cam;
		for (auto& obj : objects)
			delete obj;
		objects.resize(0);
		cam = nullptr;
	}

	void scene::SceneGLTF::OnUpdate(float deltaTime)
	{
		input::InputHandler* inputManager = input::InputHandler::Instance();
		// Read input
		float right = 0, up = 0, forward = 0;
		if (inputManager->keyboard.held[input::Key::W])
			forward = 1;
		if (inputManager->keyboard.held[input::Key::S])
			forward = -1;
		if (inputManager->keyboard.held[input::Key::D])
			right = 1;
		if (inputManager->keyboard.held[input::Key::A])
			right = -1;
		if (inputManager->keyboard.held[input::Key::E])
			up = -1;
		if (inputManager->keyboard.held[input::Key::Q])
			up = 1;

		if (inputManager->mouse.held[input::MouseButton::right])
			cam->Freefly(glm::vec3(right, up, forward),10,1, inputManager->mouse.dx, inputManager->mouse.dy, deltaTime);
	}

	void scene::SceneGLTF::OnRender()
	{
#pragma region Rendering main loop

		shader->Bind();
		gBuffer->WriteBind();
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		//glFrontFace(GL_CW);
		
		//Geometry pass
		glDepthFunc(GL_LESS);
		
		//Render out objects
		for (auto& obj : objects)
			obj->Draw(*shader, *cam);

		//LightPass
		glCullFace(GL_FRONT);

		lightShader->Bind();
		gBuffer->ReadBind();
		lightBuffer->WriteBind();
		
		for (int i = 0; i < gBuffer->GetTextures().size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, gBuffer->GetTextures()[i]);
		}
	
		glClear(GL_COLOR_BUFFER_BIT);
		glDepthFunc(GL_GREATER);
		lightShader->SetUniformVec2("resolution", glm::vec2(windowDimension.x, windowDimension.y));
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);
		LightManager::Get()->PointUpdate(*lightShader,*cam);
		glDisable(GL_BLEND);

		Framebuffer::BindDefault();
		//Render 
		glCullFace(GL_BACK);
		glDepthFunc(GL_ALWAYS);
		quadShader->Bind();
		
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, lightBuffer->GetTextures()[0]);

		for (int i = 0; i < gBuffer->GetTextures().size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, gBuffer->GetTextures()[i]);
		}
		
		Sun->Update(*quadShader, *cam);
		quadShader->SetUniformVec3("camPos", cam->GetPosition());
		quad->Draw(*quadShader);

		if (RenderDebug)
			Debug::RenderDebug(*cam);
		else
			Debug::ClearQueue();
		
#pragma endregion

	}

	void scene::SceneGLTF::OnImGUIRender()
	{
		bool show = true;
		int totalLightCount = LightManager::Get()->GetLightCount() - 1;
		static int LightObjectInt = 0;

		static bool bOrbitActive = false;

		ImGui::Begin("Scene ObjectLight", &show, ImGuiWindowFlags_NoSavedSettings);
		
		ImGui::ListBoxHeader("Objects");
		for (int n = 0; n < objects.size(); n++)
		{
			const bool selected = (inspectorSelected == n);

			if (ImGui::Selectable(objects[n]->GetName().c_str(), selected))
				inspectorSelected = n;
		}

		ImGui::ListBoxFooter();
		ImGui::DragFloat3("Position", &objects[inspectorSelected]->GetPosition()[0],0.05f);
		ImGui::DragFloat3("Rotation", &objects[inspectorSelected]->GetRotation()[0], 0.05f);


		ImGui::SliderInt("Current Light", &LightObjectInt, 0, totalLightCount);
		ImGui::DragFloat3("Light Position", &LightManager::Get()->GetLight(LightObjectInt)->position[0]);
		ImGui::ColorEdit4("Light color", &LightManager::Get()->GetLight(LightObjectInt)->color[0]);
		ImGui::DragFloat("Light intensity", &LightManager::Get()->GetLight(LightObjectInt)->intensity,0.1f);

		ImGui::Checkbox("Enable Debug Draw", &RenderDebug);
		ImGui::Checkbox("OrbitLight", &LightManager::Get()->GetLight(LightObjectInt)->bOrbit);
		
		ImGui::SetWindowPos(ImVec2(0, 100));
		ImGui::SetWindowSize(ImVec2(400, 300));
		ImGui::End();
	}
}