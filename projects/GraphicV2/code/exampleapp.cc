//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2022 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------

#include <iostream>

#include "config.h"
#include "exampleapp.h"
#include <imgui.h>


//Scene environment //header file included scene
#include "scene.h"
#include "sceneGLTF.h"

#include "inputHandler.h"
#include "gltfLoader.h"
#include "meshObjectManager.h"

//Scenes setup
scene::Scene* currentScene = nullptr;
scene::SceneMenu* sceneMenu;

input::InputHandler* inputManager = input::InputHandler::Instance();

using namespace Display;
namespace Example
{
//------------------------------------------------------------------------------
/**
*/
ExampleApp::ExampleApp()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ExampleApp::~ExampleApp()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool
ExampleApp::Open()
{
	App::Open();
	//Windows size
	static vec2 windowSize(1920, 1080);
	this->window = new Display::Window;
	window->SetSize(windowSize.x, windowSize.y);
	
	if (this->window->Open())
	{
		this->window->SetKeyPressFunction([this](int32 key, int32, int32 action, int32)
		{
			inputManager->HandleKeyEvent(key, action);
			if (key == GLFW_KEY_ESCAPE)
				this->window->Close();
		});
		
		this->window->SetMouseMoveFunction([this](float64 x, float64 y) 
		{
			inputManager->HandleMousePosition(x, y, windowSize.x, windowSize.y);
		});

		this->window->SetMousePressFunction([this](int32 button, int32 action, int32) 
		{
			inputManager->HandleMouseEvent(button, action);
		});

		return true;
	}

	return false;
}

//------------------------------------------------------------------------------
/**
*/
void
ExampleApp::Close()
{	
	if (this->window->IsOpen())
		this->window->Close();
	if (currentScene != sceneMenu)
		delete sceneMenu;
	GLTFLoader::Delete();
	MeshObject::Delete();
	Core::App::Close();
}

//------------------------------------------------------------------------------
/**
*/
void
ExampleApp::Run()
{
	//Scenes
	sceneMenu = new scene::SceneMenu(currentScene);
	currentScene = sceneMenu;

	sceneMenu->RegisterScene<scene::SceneGLTF>("Object GLTF");

	this->window->SetUiRender([this]()
	{
		this->RenderUI(*currentScene);
	});

	//------------------------------------------------------------------------------------------------------

	glEnable(GL_DEPTH_TEST);
	while (this->window->IsOpen())
	{
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		inputManager->BeginFrame();
		this->window->Update();

		if(currentScene)
		{
			currentScene->OnRender();
			currentScene->OnUpdate(ImGui::GetIO().DeltaTime);
		}
		this->window->SwapBuffers();

#ifdef CI_TEST
		// if we're running CI, we want to return and exit the application after one frame
		// break the loop and hopefully exit gracefully
		break;
#endif
	}
}

void
ExampleApp::RenderUI(scene::Scene& test)
{
	if (&test && this->window->IsOpen()) //current scene ! null
	{
		bool show = true;
		ImGui::Begin("Scene Meny", &show, ImGuiWindowFlags_NoSavedSettings);
		if (currentScene != sceneMenu && ImGui::Button("<--"))
		{
			delete currentScene;
			currentScene = sceneMenu;
		}
		currentScene->OnImGUIRender();
		ImGui::End();
	}
}

} // namespace Example