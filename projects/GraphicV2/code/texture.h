#pragma once

#include <vector>
#include <string>

namespace Object
{
	class Mesh;
}

enum TextureType
{
	BaseColor,
	Normal,
	MetallicRoughness,
	Emissive,
	Occlusion,

	NUM_TEXTURES
};

enum TextureDefault
{
	DEFAULT_WHITE,
	DEFAULT_BLACK,
	DEFAULT_NORMAL,
	NUM_DEFAULTS
};

class Texture
{
private:
	static std::vector<Texture> defaultTextures;
	static int embeddedID; 
	unsigned int renderID;
	std::string filePath;
	unsigned char* localBuffer; //Local storage for the texture
	int width, height, bpp; //bpp = bits per pixel
	TextureType type = BaseColor;

	Texture(const unsigned char* p_imageData);
	static void InitDefault();

public:
	Texture(){}
	Texture(const std::vector<unsigned char>& p_imageData);
	Texture(const std::string& p_path, bool p_bFlip = true);

	~Texture();

	void Bind() const;
	void UnBind() const;
	void Delete() const;

	inline void SetType(TextureType p_type) { this->type = p_type; }
	inline int GetWidth() { return width; }
	inline int GetHeight() { return height; }
	inline unsigned int GetTextureHandleID() const { return renderID; }


	static Texture GetDefualtWhite();
	static Texture GetDefualtBlack();
	static Texture GetDefualtNormal();

};