#include "VBO.h"

//VBO::VBO(GLfloat* vertices, GLsizeiptr size) {
//	// Generate the VBO 
//	glGenBuffers(1, &ID);
//	// Bind the VBO specifying it's a GL_ARRAY_BUFFER
//	glBindBuffer(GL_ARRAY_BUFFER, ID);
//	// Introduce the verticies into the VBO
//	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
//}

// Constructor that generates a Vertex Buffer Object and links it to vertices
void VBO::SetVBO(GLfloat* vertices, GLsizeiptr size) {
	// Generate the VBO 
	glGenBuffers(1, &ID);
	// Bind the VBO specifying it's a GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	// Introduce the verticies into the VBO
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

// Binds the VBO
void VBO::Bind() {
	// Bind the VBO specifying it's a GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

// Unbinds the VBO
void VBO::Unbind() {
	// Unbind the VBO, so no accident modification to VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Deletes the VBO
void VBO::Delete() {
	// Delete VBO when we don't need it anymore
	glDeleteBuffers(1, &ID);
}