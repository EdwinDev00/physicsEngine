//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2022 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "exampleapp.h"
#include <cstring>
#include <render/grid.h>

#include "GraphicsNode.h"
#include "Camera.h"
#include "InputManager.h"


using namespace Display;
namespace Example
{
	//Windows size
	int width = 1023;
	int height = 765;

	InputManager input;

	//Mesh ref
	//Graphics node move it to constructor (creates new shared ptr for the created instance)
	//std::shared_ptr<GraphicsNode> resourceObj = std::make_shared<GraphicsNode>(); //test smart pointer
	//std::shared_ptr<GraphicsNode> resourceObj2 = std::make_shared<GraphicsNode>(); //test smart pointer

	std::vector <std::shared_ptr<GraphicsNode>> objects; 

	Camera virtualCamera(width, height, vec3(0, 0.75f, 3));

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
	this->window = new Display::Window;
	window->SetKeyPressFunction([this](int32 key, int32, int32, int32)
	{
		if (key == GLFW_KEY_ESCAPE)
			this->window->Close();
		input.inputsFromkeyboard(key, objects[0]);
		virtualCamera.Inputs(key);
	});

	window->SetMousePressFunction([this](int32 key, int32, int32)
	{
		input.inputsFromMouse(key, objects[0], window->GetWindow(), width, height);
	});

	if (this->window->Open())
	{
		// Set clear color to gray
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		// Local share_ptr destroys when exiting this scope
		std::shared_ptr<ShaderResource> shader_ptr = std::make_shared<ShaderResource>();
		std::shared_ptr<GraphicsNode> shared = std::make_shared<GraphicsNode>();
		std::shared_ptr<GraphicsNode> shared2 = std::make_shared<GraphicsNode>();
		std::shared_ptr<GraphicsNode> test = std::make_shared<GraphicsNode>();

		shader_ptr->GetShaderResource("..\\projects\\Obj_lightingProject\\code\\vertexSourceShader.glsl"
			, "..\\projects\\Obj_lightingProject\\code\\fragmentSourceShader.glsl");
		
		shared->s_shader = shader_ptr;
		shared2->s_shader = shader_ptr;
		test->s_shader = shader_ptr;
		//-----------------------------------------------------------------------------------------
		// Generate Mesh
		shared->mesh = MeshResource::createCube(0.25f, 0.25f, 0.25f);
		// ---------------------------------------------------------------------------------------
		
		// Send in texture to the program
		shared->texture.LoadTexture("..\\projects\\Obj_lightingProject\\texture\\tile.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
		// Set the value to the texture uniform
		shared->texture.texUnit(shared->s_shader->ID, "tex0", 0);
		//----------------------------------------------------------------------------------------
		
		// Generate Mesh 2
		shared2->mesh = MeshResource::createCube(0.25f, 0.55f, 0.25f);
		// ---------------------------------------------------------------------------------------

		// Send in texture to the program
		shared2->texture.LoadTexture("..\\projects\\Obj_lightingProject\\texture\\tile.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
		// Set the value to the texture uniform
		shared2->texture.texUnit(shared->s_shader->ID, "tex0", 0);
		//----------------------------------------------------------------------------------------

		// TESTING OBJ READING
		//test->mesh.readObj1("..\\projects\\Obj_lightingProject\\obj\\cube.obj");
		test->mesh.testobjVertex = MeshResource::readObj1("..\\projects\\Obj_lightingProject\\obj\\cube.obj");
		//Setup the object BUFFER
		test->mesh.initObj();


		//std::move is used to indicate that an object t may be "moved from", i.e.allowing the efficient transfer of resources from t to another object.
		objects.push_back(std::move(shared));
		objects.push_back(std::move(shared2));
		objects.push_back(std::move(test));
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
	objects.clear(); //Clear all contents in the vector
	if (this->window->IsOpen())
		this->window->Close();

	Core::App::Close();
}

//------------------------------------------------------------------------------
/**
*/
void
ExampleApp::Run()
{
	Render::Grid Grid;
	glEnable(GL_DEPTH_TEST);
	while (this->window->IsOpen())
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		this->window->Update();
		
		// Draw out objects //FUTURE: CHANGE SO IT DOESN'T TAKE ITSELF AS ARGUMENT
		objects[0]->drawMesh(objects[0]);
		objects[1]->translateMesh(vec3(1, -0.2f, 0));
		objects[1]->drawMesh(objects[1]);
		objects[2]->drawObj(objects[2]);
		//---------------------------------------------------

		// Setup perspective proj & view frustrum 
		virtualCamera.Lookat(80.0f, 0.1f, 200.0f, objects[0]->s_shader);
		//----------------------------------------------------------------------------
		
		//Setup Grid
		Grid.Draw(&virtualCamera.projview[0][0]);
		//-----------------------------------------------------------------------------
		
		this->window->SwapBuffers();

#ifdef CI_TEST
		// if we're running CI, we want to return and exit the application after one frame
		// break the loop and hopefully exit gracefully
		break;
#endif
	}
}

} // namespace Example