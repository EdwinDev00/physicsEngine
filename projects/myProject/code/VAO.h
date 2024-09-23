#pragma once
#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include <GL/glew.h>
#include "VBO.h"

class VAO {
	public:
		GLuint ID;
		
		VAO();
		// Generate a VAO ID
		void SetVAO();
		// Links a VBO to the VAO using a certain layout
		void LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
		// Binds the VAO
		void Bind();
		// Unbinds the VAO
		void Unbind();
		// Deletes the VAO
		void Delete();
};

#endif // !VAO_CLASS_H
