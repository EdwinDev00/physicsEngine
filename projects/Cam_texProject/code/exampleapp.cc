//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2022 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "exampleapp.h"
#include <cstring>
#include <render/grid.h>

#include "MeshResource.h"
#include "TextureResource.h"
#include "Camera.h"
#include "core/math/mat4.h"

using namespace Display;
namespace Example
{
	//Windows size
	int width = 800;
	int height = 800;

	//Mesh ref
	MeshResource mesh;
	TextureResource texture;
	Camera camera(width, height, vec3(0, 1, 20));
	
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
	});


	//test
	//Vertex vertices[] =
	//{
	//	//           Positon		  /	      Color     / Texture Coord
	//	Vertex{vec3(-0.5f, -0.5f, -1), vec4(1, 0, 0, 1), 1,0},   //pos 0 //Lower Left corner quad
	//	Vertex{vec3( 0.5f, -0.5f, -1), vec4(0, 1, 0, 1), 0,1},	//pos 1 //Lower Right corner quad
	//	Vertex{vec3( 0.5f,	0.5f, -1), vec4(0, 0, 1, 1), 0,0},	//pos 2 //Upper Right corner quad
	//	Vertex{vec3(-0.5f,	0.5f, -1), vec4(1, 0, 1, 1), 1,1}    //pos 3 //Upper left corner quad		
	//};

	//GLuint indices[] =
	//{
	//	0, 1, 2, // Lower Triangle
	//	0, 3, 2, // Upper Triangle
	//};


	if (this->window->Open())
	{
		// set clear color to gray
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	
		//mesh = MeshResource::createCube(0.25f, 0.25f, 0.25f); //without destructor
		mesh.createCube1(0.25f, 0.25f, 0.25f); // with destructor (NEW) 

		//own mesh definition
		/*std::vector<Vertex>verts(vertices, vertices + sizeof(vertices) / sizeof(Vertex));
		std::vector<GLuint>indices(indices, indices + sizeof(indices) / sizeof(GLuint));
		mesh = MeshResource::OCMesh(verts, indices);*/

		texture.LoadTexture("..\\projects\\Cam_texProject\\texture\\tile.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
		texture.texUnit(mesh.ShaderProgram, "tex0", 0);

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
		
		// Tell OpenGL which Shader Program we want to use
		glUseProgram(mesh.ShaderProgram);

		//Bind Texture
		texture.Bind();

		//Virtual Camera
		camera.Orbit(&mesh);
		
		// Bind the VAO so OpenGL knows to use it
		mesh.BindVArrayBuffer();

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, mesh.MeshIndices.size(), GL_UNSIGNED_INT, 0);
		
		//mat4 viewProj = proj * view;
		Grid.Draw(&camera.projview[0][0]);

		this->window->SwapBuffers();

#ifdef CI_TEST
		// if we're running CI, we want to return and exit the application after one frame
		// break the loop and hopefully exit gracefully
		break;
#endif
	}
}

} // namespace Example