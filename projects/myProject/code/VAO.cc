#include "VAO.h"

VAO::VAO(){ 
	//glGenVertexArrays(1, &ID); 
}

//Generate the VAO
void VAO::SetVAO() {
	glGenVertexArrays(1, &ID);
}

// Links a VBO to the VAO using a certain layout
void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset) {
	VBO.Bind();
	// Configure the Vertex Attribute so that OpenGL knows how to read the VBO
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
	// Enable the Vertex Attribute so that OpenGL knows to use it
	glEnableVertexAttribArray(layout);
	VBO.Unbind();
}

void VAO::Bind() {
	// Make the VAO the current Vertex Array Object by binding it
	glBindVertexArray(ID);
}

void VAO::Unbind() {
	// Unbind VAO, for no accidently modifying
	glBindVertexArray(0);
}

void VAO::Delete() {
	glDeleteVertexArrays(1, &ID);
}