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
#include "physicsEngine.h"
#include "inlineFunc.h"


glm::vec3 hitPoint; //global for debugging otherwise move it back inside left mouse

namespace scene
{
	inline std::shared_ptr<Ray> SceneGLTF::CreateRayFromMouse()
	{
		input::InputHandler* inputManager = input::InputHandler::Instance();
		float x = inputManager->mouse.nx;
		float y = inputManager->mouse.ny;
		glm::vec2 mouseNDC(x, y);

		glm::mat4 inverseProj = glm::inverse(cam->GetProjMat());
		glm::mat4 inverseView = glm::inverse(cam->GetViewMat());

		//Convert NDC to world space coordinate
		glm::vec4 rayClip = glm::vec4(mouseNDC.x, -mouseNDC.y, -1, 1);
		glm::vec4 rayEye = inverseProj * rayClip;
		rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

		glm::vec4 rayWorld = inverseView * rayEye;
		glm::vec3 rayDir = glm::normalize(glm::vec3(rayWorld));

		glm::vec3 rayOrigin = cam->GetPosition();
		return std::make_shared<Ray>(rayOrigin, rayDir);
	}

	scene::SceneGLTF::SceneGLTF()
	{
		//Projection matrix ratio: 16:9 //Model view proj matrix
		phyEngine = new PhysicsEngine();

		cam = new Object::Camera();
		cam->Setpos(glm::vec3(0, 5, 0));
		cam->SetPerspective(30, 1920, 1080);
		cam->SetView(glm::lookAt(cam->GetPosition(), glm::vec3(0, -1, 0), cam->GetUP()));

		SetDimension(1920, 1080);
		gBuffer = std::make_shared<GBuffer>(windowDimension.x, windowDimension.y);
		lightBuffer = std::make_shared<LightBuffer>(windowDimension.x, windowDimension.y);

		
		quad = std::make_shared<Quad>();

		GameObject* ground = new GameObject(glm::vec3(0, -1, 0), glm::vec3(0, 0, 0), glm::vec3(7.0f, 1.0f, 7.0f), "../projects/GraphicV2/asset/Cube/Cube.gltf", true);
		GameObject* CubeGLTF = new GameObject(glm::vec3(2, 2, -2), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), "../projects/GraphicV2/asset/Cube/Cube.gltf");
		GameObject* CubeGLTF2 = new GameObject(glm::vec3(2, 2, 2), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), "../projects/GraphicV2/asset/Cube/Cube.gltf");
		
		//GameObject* helmGLTF = new GameObject(glm::vec3(2, 2, 2), glm::vec3(0, 0, 0), glm::vec3(3, 3, 3), "../projects/GraphicV2/asset/FlightHelmet/FlightHelmet.gltf");

		phyEngine->AddObject(ground);
		phyEngine->AddObject(CubeGLTF);
		phyEngine->AddObject(CubeGLTF2);

		shader = std::make_shared<ShaderResource>("../projects/GraphicV2/code/gBuffer.glsl");
		quadShader = std::make_shared<ShaderResource>("../projects/GraphicV2/code/quad.glsl");
		lightShader = std::make_shared<ShaderResource>("../projects/GraphicV2/code/LightPass.glsl");
 
		//Directional light (sun)
		Sun = new DirectionalLight(glm::vec3(0, -1, 0), glm::vec3(.5f, .5f, .5f),1.0f);

		//testing the ray to spawn
		ray = std::make_shared<Ray>(cam->GetPosition(), cam->GetDirection()); //Just for debug ray (change it back to local when works)

		Debug::Init();
	}

	scene::SceneGLTF::~SceneGLTF()
	{	
		//Deallocate the memory
		LightManager::Get()->Clear();
		delete cam;
		phyEngine->DeleteObjectData();
		delete phyEngine;
		phyEngine = nullptr;
		cam = nullptr;
	}

	void scene::SceneGLTF::OnUpdate(float deltaTime)
	{
		phyEngine->Update(deltaTime);

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

		if (inputManager->mouse.pressed[input::MouseButton::left])
		{
			ray = CreateRayFromMouse();
			
			/*GameObject**/ hitObject = phyEngine->Raycast(*ray,hitPoint);

			if(hitObject)
			{
				phyEngine->ApplyForce(hitObject, ray->direction, forceMagnitude,hitPoint);
			}
		}

		//Debug only RAY 
		glm::vec3 rayEnd = ray->origin + ray->direction * 100.0f;  // Scale to a reasonable length
		if(hitObject)
		{
			Debug::DrawLine(ray->origin, rayEnd, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));  // Red color for the ray
			Debug::DrawBox(hitPoint, glm::vec3(), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec4(0, 1, 0, 1), 2);
		}
		else
			Debug::DrawLine(ray->origin, rayEnd, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));  // Red color for the ray
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
		for (auto& obj : phyEngine->objects)
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
		for (int n = 0; n < phyEngine->objects.size(); n++)
		{
			const bool selected = (inspectorSelected == n);

			if (ImGui::Selectable(phyEngine->objects[n]->GetName().c_str(), selected))
				inspectorSelected = n;
		}

		ImGui::ListBoxFooter();
		if(phyEngine->objects[inspectorSelected]->mass != 0)
		{
			ImGui::DragFloat3("Angular velocity", &phyEngine->objects[inspectorSelected]->angularVelocity[0], 0.05f);
			ImGui::DragFloat3("Velocity", &phyEngine->objects[inspectorSelected]->velocity[0], 0.05f);
		}

		ImGui::DragFloat3("Position", &phyEngine->objects[inspectorSelected]->GetPosition()[0],0.01f);
		/*ImGui::DragFloat3("Rotation", &phyEngine->objects[inspectorSelected]->GetRotation()[0], 0.05f);
		ImGui::DragFloat3("Scale", &phyEngine->objects[inspectorSelected]->GetScale()[0], 0.05f);*/

		ImGui::DragFloat("Force Magnitude", &forceMagnitude, 0.1f);
		ImGui::DragFloat3("Gravity Force", &phyEngine->gravity[0], 0.1f);

		ImGui::ColorEdit4("Light color", &LightManager::Get()->GetLight(LightObjectInt)->color[0]);
		ImGui::DragFloat("Light intensity", &LightManager::Get()->GetLight(LightObjectInt)->intensity,0.1f);

		ImGui::Checkbox("Enable Debug Draw", &RenderDebug);
		
		ImGui::SetWindowPos(ImVec2(0, 100));
		ImGui::SetWindowSize(ImVec2(500, 500));
		ImGui::End();
	}
}