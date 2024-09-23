#pragma once

#include "MeshResource.h"

#pragma region VBO_Properties

//Generate VBO and bind it to GL_ARRAY_BUFFER
void MeshResource::GenerateVBuffer(GLfloat* vertices, GLsizeiptr size) {
	// Generate the VBO with only 1 object each
	glGenBuffers(1, &VBO);
	// Bind the VBO specifying it's a GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Introduce the vertices into the VBO
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

//Test
//Generate VBO and bind it to GL_ARRAY_BUFFER
void MeshResource::GenerateVBuffer(std::vector<Vertex>& vertices) {

	// Generate the VBO with only 1 object each
	glGenBuffers(1, &VBO);
	// Bind the VBO specifying it's a GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Introduce the vertices into the VBO
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}
#pragma endregion

#pragma region EBO_Properties
void MeshResource::GenerateEBuffer(GLuint* indices, GLsizeiptr size) {
	glGenBuffers(1, &EBO); // Generate the EBO 
	// Bind the EBO specifying it's a GL_ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// Introduce the indices into the EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}

void MeshResource::GenerateEBuffer(std::vector<GLuint>& indices) {
	glGenBuffers(1, &EBO); // Generate the EBO 
	// Bind the EBO specifying it's a GL_ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// Introduce the indices into the EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}
#pragma endregion

#pragma region VAO_Properties

void MeshResource::GenerateVArray() {
	// Generate the VAO with only 1 object each
	glGenVertexArrays(1, &VAO);
}

void MeshResource::LinkAttrib(GLuint& VBO, GLuint index, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset) {
	// Vertex Buff / specified vertex attribute to be modified
	BindArrayBuffer(VBO);
	glVertexAttribPointer(index, numComponents, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(index);
	UnbindVBuffer();
}
#pragma endregion


#pragma region General_Functionality

#pragma region Binds & Unbinds

void MeshResource::BindArrayBuffer(GLuint buffer) {
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
}

void MeshResource::BindVArrayBuffer() {
	glBindVertexArray(VAO);
}

void MeshResource::UnbindVBuffer() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshResource::UnbindEBuffer() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MeshResource::UnbindVertexArray() {
	glBindVertexArray(0);
}

void MeshResource::UnbindEverything() {
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
#pragma endregion


#pragma endregion


#pragma region MeshCreations
//Pre Mesh samples
MeshResource MeshResource::createCube(float width, float height, float depth) {
	MeshResource newMesh;
	//Vertices & indices Data
	Vertex vertices[] =
	{
		//			Position		/			Colour				/	Texture Coord
			//Front
		Vertex{vec3(-width,-height, depth), vec4(1.0f, 0.0f, 0.0f, 1.0f),  0.0f, 0.0f},   // v0
		Vertex{vec3( width,-height, depth), vec4(0.0f, 1.0f, 0.0f, 1.0f),  1.0f, 0.0f},   // v1
		Vertex{vec3( width, height, depth), vec4(0.0f, 0.0f, 1.0f, 1.0f),  1.0f, 1.0f},   // v2
		Vertex{vec3(-width, height, depth), vec4(1.0f, 1.0f, 1.0f, 1.0f),  0.0f, 1.0f},   // v3
		//Back															   
		Vertex{vec3(-width,-height,-depth), vec4(1.0f, 0.0f, 0.0f, 1.0f),  0.0f, 0.0f},   // v4
		Vertex{vec3( width,-height,-depth), vec4(0.0f, 1.0f, 0.0f, 1.0f),  1.0f, 0.0f},   // v5
		Vertex{vec3( width, height,-depth), vec4(0.0f, 0.0f, 1.0f, 1.0f),  1.0f, 1.0f},   // v6
		Vertex{vec3(-width, height,-depth), vec4(1.0f, 1.0f, 1.0f, 1.0f),  0.0f, 1.0f},   // v7
	};

	GLuint indices[] =
	{
		 //Front
		 0,1,2, 2,0,3,
		 //Right
		 1,5,6, 6,2,1,
		 //Back
		 7,6,5, 5,4,7,
		 //Left
		 4,0,3, 3,7,4,
		 //Bottom
		 4,5,1, 1,0,4,
		 //Top
		 3,2,6, 6,7,3
	};

	//set the vertices to the vector array
	std::vector<Vertex>v(vertices, vertices + sizeof(vertices) / sizeof(Vertex));
	std::vector<GLuint>i(indices, indices + sizeof(indices) / sizeof(GLuint));

	newMesh.MeshVertices = v;
	newMesh.MeshIndices = i;

	//Setup the mesh // move this to a draw call later on
	newMesh.GenerateVArray();
	newMesh.BindVArrayBuffer();

	newMesh.GenerateVBuffer(newMesh.MeshVertices);

	newMesh.GenerateEBuffer(newMesh.MeshIndices);

	newMesh.LinkAttrib(newMesh.VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	newMesh.LinkAttrib(newMesh.VBO, 1, 4, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	newMesh.LinkAttrib(newMesh.VBO, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)(7 * sizeof(float)));

	newMesh.UnbindEverything();

	return newMesh;
}

//Create own Meshes by passing vertices & indices (using Vertex struct)
MeshResource MeshResource::OCMesh(std::vector<Vertex>& inVertices, std::vector<GLuint>& inIndices) {

	MeshResource newMesh; //Create newMesh

	newMesh.MeshVertices = inVertices;
	newMesh.MeshIndices = inIndices;

	//Setup the mesh //Probally move this to a draw call later
	newMesh.GenerateVArray();
	newMesh.BindVArrayBuffer();

	newMesh.GenerateVBuffer(inVertices);

	newMesh.GenerateEBuffer(inIndices);

	newMesh.LinkAttrib(newMesh.VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	newMesh.LinkAttrib(newMesh.VBO, 1, 4, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	newMesh.LinkAttrib(newMesh.VBO, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)(7 * sizeof(float)));

	newMesh.UnbindEverything();

	return newMesh; //returns the custom made mesh
}

#pragma endregion


