#include "TextureResource.h"

void TextureResource::LoadTexture(const char* imagePath, GLenum texType, GLuint slot, GLenum format, GLenum pixelType)
{
	// Assigns the type of the texture to the texture object
	type = texType;

	// Stores the width, height, and the number of color channels of the image
	int widthImg, heightImg, numColCh;

	// Flips the image so it appears right side up
	stbi_set_flip_vertically_on_load(true);
	// Reads the image from a file and stores it in bytes
	unsigned char* imageBytes = stbi_load(imagePath, &widthImg, &heightImg, &numColCh, STBI_rgb_alpha); //can't set it to 0

	// Generates an OpenGL texture object
	glGenTextures(1, &ID);
	// Assigns the texture to a Texture Unit
	glActiveTexture(slot);
	glBindTexture(texType, ID);

	// Configures the type of algorithm that is used to make the image smaller or bigger
	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Configures the way the texture repeats (if it does at all)
	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//error handling
	if (imageBytes)
	{
		// Assigns the image to the OpenGL Texture object
		glTexImage2D(texType, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType, imageBytes);
		// Generates MipMaps
		glGenerateMipmap(texType); //Can't generate the mipmap
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	// Deletes the image data as it is already in the OpenGL Texture object
	stbi_image_free(imageBytes);

	// Unbinds the OpenGL Texture object so that it can't accidentally be modified
	glBindTexture(texType, 0);
}

void TextureResource::texUnit(GLuint& shaderProgram, const char* uniform, GLuint unit)
{
	// Gets the location of the uniform
	GLuint tex0Uni = glGetUniformLocation(shaderProgram, uniform); //Crash here
	// Shader needs to be activated before changing the value of a uniform
	glUseProgram(shaderProgram);
	// Sets the value of the uniform
	glUniform1i(tex0Uni, unit);
}

void TextureResource::Bind() 
{
	glBindTexture(type, ID);
}

void TextureResource::Unbind()
{
	glBindTexture(type, 0);
}

void TextureResource::Delete()
{
	glDeleteTextures(1,&ID);
}