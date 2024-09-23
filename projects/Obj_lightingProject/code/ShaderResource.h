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
		~ShaderResource(){}

		// Reference ID of the Shader Program
		GLuint ID;

		void GetShaderResource(const char* vertexFPath, const char* fragmentFPath);
	
		//getUniforms
		void setMat4Uniform(const char* uniform, float* value);
		void setVec4Uniform(const char* uniform, float* value);

		// Activates the Shader Program
		void Activate();
		// Deletes the Shader Program
		void Delete();
};
#endif // !SHADERRESOURCE_CLASS_H
