#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <string>

//file reading library
//input/output stream class to operate on files
#include <fstream>	
#include <sstream>	
#include <iostream>	

#include <cerrno> //error handling library

#include <GL/glew.h> //OpenGL library to use GL properties

//Read the shader text file
std::string get_file_contents(const char* fileName);

class Shader {
	public:
	// Reference ID of the Shader Program
		GLuint ID; 
		Shader() {/* Empty*/ };
		
		//Shader(const char* vertexFile, const char* fragmentFile); //may need to be a function
		
		//That build the Shader program from 2 different shaders
		void LoadShaderFile(const char* vertexFile, const char* fragmentFile); 
		// Activates the Shader Program
		void Activate(); 
		// Deletes the Shader Program
		void Delete(); 
};
#endif