//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2022 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "exampleapp.h"
#include <cstring>
#include "MeshResource.h"
#include "core/math/mat4.h"

using namespace Display;
namespace Example
{
	//Mesh ref
	MeshResource mesh;

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
	window->SetKeyPressFunction([this](int32, int32, int32, int32)
	{
		this->window->Close();
	});

	// Vertices for the mesh
	GLfloat vertices[] =
	{
		//Positon			/	  Color
		-0.5f, -0.5f, -1,		1, 0, 0, 1,   //pos 0 //Lower Left corner quad
		 0.5f, -0.5f, -1,		0, 1, 0, 1,	  //pos 1 //Lower Right corner quad
		 0.5f,	0.5f, -1,		0, 0, 1, 1,	  //pos 2 //Upper Right corner quad
		-0.5f,	0.5f, -1,	    1, 0, 1, 1   //pos 3 //Upper left corner quad		
	};

	// indices for the mesh
	GLuint indices[] =
	{
		0, 1, 2, // Lower Triangle
		0, 3, 2, // Upper Triangle
		
	};


	if (this->window->Open())
	{
		// set clear color to gray
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		// Read the data from shader file and Generate Shaders and program
		mesh.ReadShaderFile("..\\projects\\testProject\\code\\vertexSourceShader.glsl"
			, "..\\projects\\testProject\\code\\fragmentSourceShader.glsl");


		//Generate VAO & bind it
		mesh.GenerateVArray();
		mesh.BindVArrayBuffer();

		//Create the VBO and links it to vertices
		mesh.GenerateVBuffer(vertices,sizeof(vertices));

		mesh.GenerateEBuffer(indices, sizeof(indices));


		//Links the VBO to VAO
		mesh.LinkAttrib(mesh.VBO, 0, 3, GL_FLOAT, 7*sizeof(float), (void*)0);
		mesh.LinkAttrib(mesh.VBO, 1, 4, GL_FLOAT, 7*sizeof(float), (void*) (3 * sizeof(float)));
		
		
		//Get the uniform reference
		mesh.uniTransID = glGetUniformLocation(mesh.ShaderProgram, "transform");
		mesh.uniVTransID = glGetUniformLocation(mesh.ShaderProgram, "transformVec");
		

		//Unbind both VAO & VBO
		mesh.UnbindEverything();


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
	while (this->window->IsOpen())
	{
		glClear(GL_COLOR_BUFFER_BIT);
		this->window->Update();
		
		// Tell OpenGL which Shader Program we want to use
		glUseProgram(mesh.ShaderProgram);

		//Rotation of the mesh
		mesh.transform = mesh.transform * rotationz(0.1f);
		// translate the mesh
		mesh.translatePos(vec3(sin(glfwGetTime()),0,0)); 
		//Bind the transform uniform & translation
		glUniformMatrix4fv(mesh.uniTransID, 1, GL_FALSE, &mesh.transform.m[0][0]);
		
		// Bind the VAO so OpenGL knows to use it
		mesh.BindVArrayBuffer();

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		this->window->SwapBuffers();

#ifdef CI_TEST
		// if we're running CI, we want to return and exit the application after one frame
		// break the loop and hopefully exit gracefully
		break;
#endif
	}
}

} // namespace Example