#pragma once

#ifndef MESHRESOURCE_CLASS_H
#define MESHRESOURCE_CLASS_H


#include <fstream>
#include <iostream>
#include <sstream>	

#include <GL/glew.h>
#include "core/math/mat4.h"
#include <vector>

// TODO LOAD OBJ FILE AND BE ABLE TO DRAW OUT TO THE PROGRAM
// ATTEMPT 1 CAN BREAK OUT TO SEPERATE CLASS WHICH MESH RESOURCE HAVE ACCESS TO IT DATA
// ATTEMPT 2 DO THE LOAD OBJECT FUNCTIONS FOR READING AND DRAWING IN THE MESH CLASS

struct Vertex 
{
	vec3 vPositon;
	vec4 vColour;
	vec2 textureCoord;//float texCoord[2];
	vec3 vNormal; //add normals to the mesh data rendering
};

struct face {
	// vertex_position[0], vertex_normal[1], verex_texCoord[2]
	unsigned int v1[3], v2[3], v3[3];
};

//--------------------------------------------------------


class MeshResource {
	public:
		// Reference // Can make them private
		GLuint VBO,VAO,EBO;

		////Obj test		
		 std::vector<Vertex> testobjVertex;
		 std::vector<int> testobjIndices; //need to think about this part (get access to the indices)

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

		// Generate mesh From OBJ file
		static std::vector<Vertex> readObj1(const char* fileName);
		void initObj();
		
};
#endif // !MESHRESOURCE_CLASS_H
