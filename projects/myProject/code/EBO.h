#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include <GL/glew.h>

class EBO
{
public:
	// ID reference of Elements Buffer Object
	GLuint ID;
	//EBO(){/* Empty */ };

	// Generate a Elements Buffer Object and links it to indices
	void SetEBO(GLuint* indices, GLsizeiptr size); //may need to make it to a void function
	//   Vertices		/	size in bytes

	// Binds the EBO
	void Bind();
	// Unbinds the EBO
	void Unbind();
	// Deletes the EBO
	void Delete();
};

#endif // !VBO_CLASS_H
