#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include <GL/glew.h>

class VBO 
{
	public:
		// Reference ID of the Vertex Buffer Object
		GLuint ID; 
		
		VBO(){/* Empty*/ };
		
		//VBO(GLfloat* vertices, GLsizeiptr size);

		// Constructor that generates a Vertex Buffer Object and links it to vertices
		void SetVBO(GLfloat* vertices, GLsizeiptr size); 
		//   Vertices		/	size in bytes
		
		// Binds the VBO
		void Bind();
		// Unbinds the VBO
		void Unbind();
		// Deletes the VBO
		void Delete();
};

#endif // !VBO_CLASS_H
