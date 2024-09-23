#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include <gl/glew.h>
#include <core/stb/stb_image.h>

#include "shaderClass.h"

class Texture 
{
	public:
		GLuint ID;
		Texture(){ /*empty*/ }
		GLenum type;

		// Setup and Load the texture
		void LoadTexture(const char* imagePath, GLenum texType, GLenum slot, GLenum format, GLenum pixelType);

		// Assigns a texture unit to a texture
		void texUnit(Shader& shader, const char* uniform, GLuint unit);
		// Binds a texture
		void Bind();
		// Unbinds a texture
		void Unbind();
		// Delete the texture
		void Delete();
};
#endif // !TEXTURE_CLASS_H
