//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2022 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "exampleapp.h"
#include <cstring>

//#include "MeshResourceV2.h"

#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "texture.h"
#include "Camera.h"

//Math library
#include "core/math/mat4.h";
#include "core/math/vec4.h";
#include "core/math/vec3.h"

////texture
//#include <core/stb/stb_image.h>

using namespace Display;

namespace Example
{	
	//////texture test
	//GLuint texture;

	const unsigned int width = 800;
	const unsigned int height = 800;
	//float rotation = 0.0f;
	//double prevTime = glfwGetTime();

	//Class Objects Definition
	//Shader shaderProgram;
	Shader shaderProgram;
	Texture texture;
	VAO VAO1;
	VBO VBO1;
	EBO EBO1;
	// Creates camera object
	Camera camera(width, height, vec3(0.0f, 0.0f, 2.0f));
	
	//Vertices coordinates Quad
	GLfloat vertices[] =
	{

		//Cube
		//Front
		-0.25f, -0.25f,  0.0f,   1.0f, 0.0f, 0.0f, 1.0f,   0.0f,0.0f, //0
		 0.25f, -0.25f,  0.0f,   0.0f, 1.0f, 0.0f, 1.0f,   1.0f,0.0f, //1
		 0.25f,  0.25f,  0.0f,   0.0f, 0.0f, 1.0f, 1.0f,   1.0f,1.0f, //2
		-0.25f,  0.25f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,   0.0f,1.0f, //3
									   			 		 
		//Back						    		 		 
		-0.25f, -0.25f, -0.5f,   1.0f, 0.0f, 0.0f, 1.0f,   0.0f,0.0f, // 4
		 0.25f, -0.25f, -0.5f,   0.0f, 1.0f, 0.0f, 1.0f,   1.0f,0.0f, // 5
		 0.25f,  0.25f, -0.5f,   0.0f, 0.0f, 1.0f, 1.0f,   1.0f,1.0f, // 6
		-0.25f,  0.25f, -0.5f,   1.0f, 1.0f, 1.0f, 1.0f,   0.0f,1.0f  // 7
	};

	// indices for the indexBuffer
	GLuint indices[] =
	{
		//Cube
		//Front
		 0,1,2,
		 2,0,3,
		////Right
		 1,5,6,
		 6,2,1,
		////Back
		 7,6,5,
		 5,4,7,
		//Left
		 4,0,3,
		 3,7,4,
		//Bottom
		 4,5,1,
		 1,0,4,
		//Top
		3,2,6,
		6,7,3
	};



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
		if(key ==GLFW_KEY_ESCAPE)
			this->window->Close();
	});


	if (this->window->Open())
	{
		// set clear the color to background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);


		/*shaderProgram = Shader("C:\\Users\\chemak-1-local\\source\\repos\\LuleaUniversityOfTechnology\\s00093-lab-env-EdwinDev00\\s0009e-lab-env-EdwinDev00\\projects\\myProject\\code\\default_vert.glsl",
			"C:\\Users\\chemak-1-local\\source\\repos\\LuleaUniversityOfTechnology\\s00093-lab-env-EdwinDev00\\s0009e-lab-env-EdwinDev00\\projects\\myProject\\code\\default_frag.glsl");*/
		
		//Generate shader and fragment from the shader files
		shaderProgram.LoadShaderFile("C:\\Users\\chemak-1-local\\source\\repos\\LuleaUniversityOfTechnology\\s00093-lab-env-EdwinDev00\\s0009e-lab-env-EdwinDev00\\projects\\myProject\\code\\default_vert.glsl",
			"C:\\Users\\chemak-1-local\\source\\repos\\LuleaUniversityOfTechnology\\s00093-lab-env-EdwinDev00\\s0009e-lab-env-EdwinDev00\\projects\\myProject\\code\\default_frag.glsl"); //Absolute path might get problem when 

		//Generate vertex array object and binds it
		VAO1.SetVAO();
		VAO1.Bind();

		////VBO
		//Generate Vertex buffer object and links it to vertices
		VBO1.SetVBO(vertices, sizeof(vertices));

		////EBO
		//Generate Element buffer object and links it to indices
		EBO1.SetEBO(indices, sizeof(indices));

		//Links VBO attributes such as coordinate and colors to VAO
		//Positon
		VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 9*sizeof(float), (void*) 0);
		//Color								  
		VAO1.LinkAttrib(VBO1, 1, 4, GL_FLOAT, 9*sizeof(float), (void*)(3 * sizeof(float)));
		//Texture 2D vec2
		//check the texture
		VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 9*sizeof(float), (void*)(7 * sizeof(float)));
		
		//Unbind all to prevent accidentally modifying
		VAO1.Unbind();
		VBO1.Unbind();
		EBO1.Unbind();
		
		//Uniform 
		// - Getting inputs & outputs within and outside the shader 
		/* - Sort of universal variables that can be accessed by other shaders
			 and can even be accessed from within the main function of the program without 
			 the use of a VAO*/ 

		//Gets ID of uniform called "scale"
		//GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

		
		//----------------------------------------------------------------------------------------------------------------------------------
		//relative path approach
		/*relative path thing in order to centralize all the resources into one folder and not
		 *duplicate them between tutorial folders.*/

		/*std::string parentDir = (fs::current_path().fs::path::parent_path()).string();
		std::string texPath = "\\projects\\myProject\\code\\";*/

		/*Texture popCat((parentDir + texPath + "test2.png").c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
		popCat.texUnit(shaderProgram, "tex0", 0);*/
       // ----------------------------------------------------------------------------------------------------------------------------------
		
	   //Texture
		texture.LoadTexture("C:\\Users\\chemak-1-local\\source\\repos\\LuleaUniversityOfTechnology\\s00093-lab-env-EdwinDev00\\s0009e-lab-env-EdwinDev00\\projects\\myProject\\code\\test2.png",
			GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
		texture.texUnit(shaderProgram, "tex0", 0);

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
	// Delete all the object we've created
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	texture.Delete();
	shaderProgram.Delete();
	
	//For only testing purpose (until real input system is implemented)
	delete window;

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
	//Program will abort when closing due to incorrectly connection to the window

	while (this->window->IsOpen())
	{
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		this->window->Update();

		// Tell OpenGL which Shader Program we want to use
		shaderProgram.Activate();

		//Camera
		// Handles camera inputs
		camera.Inputs(this->window->GetWindow());
		// Updates and exports the camera matrix to the Vertex Shader
		camera.Matrix(90.0f, 0.1f, 100.f, shaderProgram, "camMatrix");
		
		// Binds texture so that is appears in rendering
		texture.Bind();

		// Bind the VAO so OpenGL knows to use it
		VAO1.Bind();

		//Draw primitives, number of indices, datatype of indices, index of indices
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, 0);

		glBindBuffer(GL_ARRAY_BUFFER,0);


		this->window->SwapBuffers();

#ifdef CI_TEST
		// if we're running CI, we want to return and exit the application after one frame
		// break the loop and hopefully exit gracefully
		break;
#endif
	}
}

} // namespace Example



	//Vertices coordinates
	//Vertex vertices[] = 
	//{  //COORDINATES         /                NORMALS             /         COLORS              /        TEXTURE COORDINATES (texCoord) //
	//	Vertex{vec3("COORDINATES XYZ"), vec3("COORDINATE XYZ"),	vec3("RGBA"), "TEXTURE 2D"; }
	//}

	//	//test vertex array
	//Vertex vertex[] = {
	//Vertex{vec3(-0.5f,	-0.5f,	-1), vec4(1,0,0,1)},
	//Vertex{vec3(0,		0.5f,	-1), vec4(0,1,0,1)},
	//Vertex{vec3(0.5f,	-0.5f,	-1), vec4(0,0,1,1)}
	//};

	////Vertices coordinates for Array
	//GLfloat buf[] =
	//{
	//	-0.5f,	-0.5f,	-1,			// pos 0
	//	1,		0,		0,		1,	// color 0
	//	0,		0.5f,	-1,			// pos 1
	//	0,		1,		0,		1,	// color 0
	//	0.5f,	-0.5f,	-1,			// pos 2
	//	0,		0,		1,		1	// color 0
	//};

	//glDrawArrays(GL_TRIANGLES, 0, 3); //Draw without indices OpenGL