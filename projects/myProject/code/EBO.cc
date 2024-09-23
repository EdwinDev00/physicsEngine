#include "EBO.h"

// Generate a Elements Buffer Object and links it to indices
void EBO::SetEBO(GLuint* indices, GLsizeiptr size) {
	// Generate the EBO 
	glGenBuffers(1, &ID);
	// Bind the EBO specifying it's a GL_ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	// Introduce the indices into the EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}

// Binds the EBO
void EBO::Bind() {
	// Bind the EBO specifying it's a GL_ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

// Unbinds the EBO
void EBO::Unbind() {
	// Unbind the EBO, so no accident modification to VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// Deletes the EBO
void EBO::Delete() {
	// Delete EBO when we don't need it anymore
	glDeleteBuffers(1, &ID);
}