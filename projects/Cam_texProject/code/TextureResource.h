#pragma once

#ifndef TEXTURERESOURCE_CLASS_H
#define TEXTURERESOURCE_CLASS_H

//Move these files to shader class
#include <string>
#include <fstream>	
#include <sstream>	
#include <iostream>	


#include <gl/glew.h>
#include <core/stb/stb_image.h>

class TextureResource
{
	public:
		GLuint ID; //texture ref
		GLenum type;

		~TextureResource() { 
			// Clean up the memory
			std::cout << "invoked texture" << std::endl; 
			Delete();
		}

		// Setup and Load the texture
		void LoadTexture(const char* imagePath, GLenum texType, GLuint slot, GLenum format, GLenum pixelType);

		// Assigns a texture unit to a texture
		void texUnit(GLuint& shaderProgram, const char* uniform, GLuint unit);

		void Bind();
		void Unbind();
		void Delete();
};
#endif // !TEXTURERESOURCE_CLASS_H
