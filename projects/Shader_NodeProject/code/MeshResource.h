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
		// Reference // Can make them private
		GLuint VBO,VAO,EBO;

		//Mesh reference
		std::vector<Vertex> MeshVertices;
		std::vector<GLuint> MeshIndices;
		
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
		void UnbindEverything();

		//Genearte preset meshes
		static MeshResource createCube(float width,float height,float depth);
		static MeshResource OCMesh( std::vector<Vertex>& inVertices, std::vector<GLuint>& inIndices);
};
#endif // !MESHRESOURCE_CLASS_H
