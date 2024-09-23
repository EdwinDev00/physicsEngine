#pragma once

#ifndef SHADERRESOURCE_CLASS_H
#define SHADERRESOURCE_CLASS_H

#include <gl/glew.h>
#include <string>

#include <fstream>
#include <iostream>
#include <sstream>

//Read the shader text file
std::string get_file_contents(const char* fileName);

class ShaderResource 
{
	public:
		// Reference ID of the Shader Program
		GLuint ID;

		//That build the Shader program from 2 different shaders
		void GetShaderResource(const char* vertexFPath, const char* fragmentFPath);
		
		//Need to add a function to modify uniforms (TODO)
		/*Add functions to modify uniform variables of
		at least the type Matrix4fv and Vector4f (or Vector4fv).*/

		//getUniforms
		void setMat4Uniform(const char* uniform, float* value);
		void setVec4Uniform(const char* uniform, float* value);


		// Activates the Shader Program
		void Activate();
		// Deletes the Shader Program
		void Delete();
};
#endif // !SHADERRESOURCE_CLASS_H
