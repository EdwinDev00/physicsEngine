#pragma once

#ifndef MESHRESOURCE_CLASS_H
#define MESHRESOURCE_CLASS_H


#include <fstream>
#include <iostream>
#include <sstream>	

#include <GL/glew.h>
#include "core/math/mat4.h"
#include <vector>


struct Vertex 
{
	vec3 vPositon;
	vec4 vColour;
	float texCoord[2];
};

//--------------------------------------------------------


class MeshResource {
public:
		~MeshResource() //(NEW)
		{
			// TODO: glDeleteBuffers
			std::cout << "invoked" << std::endl;
			glDeleteBuffers(1, &this->VBO);
			glDeleteBuffers(1, &this->EBO);
			glDeleteBuffers(1, &this->VAO);
			glDeleteProgram(this->ShaderProgram);

		}
		// Reference
		GLuint VBO,VAO,EBO;
		GLuint VertexShader;
		GLuint FragmentShader;
		GLuint ShaderProgram;
	
		//Mesh reference
		std::vector<Vertex> MeshVertices;
		std::vector<GLuint> MeshIndices;

		//Setup shaders
		GLuint SetVShader(); //Setup Vertex Shader
		GLuint SetFShader(); //Setup Fragment Shader
		GLuint SetProgramObj(); //Setup Shader Program

		
		// VBO
		void GenerateVBuffer(GLfloat* vertices, GLsizeiptr size); //Create Vbuffer Object
		void GenerateVBuffer(std::vector<Vertex>& vertices); //Create Vbuffer Object
		
		// EBO
		void GenerateEBuffer(GLuint* indices, GLsizeiptr size);
		void GenerateEBuffer(std::vector<GLuint>& indices);

		// VAO
		void GenerateVArray(); //Create Varray Object
		void LinkAttrib(GLuint& VBO, GLuint index, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
		
		//Bind Methods
		void BindArrayBuffer(GLuint buffer);
		void BindVArrayBuffer();

		//Unbind methods
		void UnbindVBuffer();
		void UnbindEBuffer();
		void UnbindVertexArray();

		//test
		void UnbindEverything();

		void DeleteShader(GLuint shader);
		//void Delete();
		void ReadShaderFile(const char* vertexFilePath, const char* fragmentFilePath);

		void translatePos(vec3 vpos);

		//Genearte preset meshes
		static MeshResource createCube(float width,float height,float depth); //(REPLACE)
		MeshResource* createCube1(float width, float height, float depth); // (NEW) this fixed with destructor
		static MeshResource OCMesh( std::vector<Vertex>& inVertices, std::vector<GLuint>& inIndices);


	private:
		const GLchar* VertexSource;
		const GLchar* FragmentSource;
};
#endif // !MESHRESOURCE_CLASS_H
