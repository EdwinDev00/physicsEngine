#include "texture.h"
#include <gl/glew.h>
#include "core/stb/stb_image.h"

int Texture::embeddedID = 0;
std::vector<Texture> Texture::defaultTextures;

Texture::Texture(const std::vector<unsigned char>& p_imageData)
	:renderID(0), filePath("embeddedTexture_" + embeddedID++), localBuffer(nullptr), width(0), height(0), bpp(0)
{
	stbi_set_flip_vertically_on_load(false);
	localBuffer = stbi_load_from_memory(p_imageData.data(), p_imageData.size(),
		&width, &height, &bpp,4);

	glGenTextures(1, &renderID);
	glBindTexture(GL_TEXTURE_2D, renderID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //Clamp Horizontal 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); //Clamp Vertical
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (localBuffer)
		stbi_image_free(localBuffer);
}

Texture::Texture(const unsigned char* p_imageData)
	:renderID(0), filePath(""), localBuffer(nullptr), width(1), height(1), bpp(0)
{

	glGenTextures(1, &renderID);
	glBindTexture(GL_TEXTURE_2D, renderID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //Clamp Horizontal 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); //Clamp Vertical
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, p_imageData);
	glBindTexture(GL_TEXTURE_2D, 0);
}


Texture::Texture(const std::string& p_path, bool p_bFlip)
	: renderID(0), filePath(p_path), localBuffer(nullptr), width(0), height(0),bpp(0)
{
	stbi_set_flip_vertically_on_load(p_bFlip);
	localBuffer = stbi_load(p_path.c_str(), &width, &height, &bpp, 4);

	glGenTextures(1, &renderID);
	glBindTexture(GL_TEXTURE_2D, renderID);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //Clamp Horizontal 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); //Clamp Vertical
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (localBuffer)
		stbi_image_free(localBuffer);
}

Texture::~Texture()
{
	//GLCall(glDeleteTextures(1, &RenderID));
}

void Texture::Bind() const
{
	glActiveTexture(GL_TEXTURE0 + type);
	glBindTexture(GL_TEXTURE_2D, renderID);
}

void Texture::UnBind() const
{
	glActiveTexture(GL_TEXTURE0 + type);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Delete() const
{
	glDeleteTextures(1, &renderID);
}

void Texture::InitDefault()
{
	defaultTextures.resize(NUM_DEFAULTS);
	const unsigned char colors[3][4]
	{
		{255,255,255,255}, //WHITE TEXTURE
		{0,0,0,255}, //BLACK 
		{128,128,255,255} //FLAT NORMAL
	};

	defaultTextures[DEFAULT_WHITE] = Texture(colors[DEFAULT_WHITE]);
	defaultTextures[DEFAULT_BLACK] = Texture(colors[DEFAULT_BLACK]);
	defaultTextures[DEFAULT_NORMAL] = Texture(colors[DEFAULT_NORMAL]);
}

Texture Texture::GetDefualtWhite()
{
	if (defaultTextures.empty())
		InitDefault();
	return defaultTextures[DEFAULT_WHITE];
}

Texture Texture::GetDefualtBlack()
{
	if (defaultTextures.empty())
		InitDefault();
	return defaultTextures[DEFAULT_BLACK];

}

Texture Texture::GetDefualtNormal()
{
	if (defaultTextures.empty())
		InitDefault();
	return defaultTextures[DEFAULT_NORMAL];
}

