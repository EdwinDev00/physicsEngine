#pragma once

#include <vector>

#include "vertexArray.h"
#include "indexBuffer.h"

#include "core/app.h"

class ShaderResource;

class Framebuffer
{
protected:
	unsigned int fbo = 0;
	unsigned int rbo = 0;
	std::vector<unsigned int> textures;
	std::vector<unsigned int> attachments;

	int windowWidth = 0, windowHeight = 0;

	virtual void Init() = 0;

	virtual ~Framebuffer(){}

public:
	inline static void BindDefault(){ glBindFramebuffer(GL_FRAMEBUFFER, 0); }
	inline static void BindDefaultWrite(){ glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); }
	inline static void BindDefaultRead(){ glBindFramebuffer(GL_READ_FRAMEBUFFER, 0); }

	inline void Bind(){ glBindFramebuffer(GL_FRAMEBUFFER, fbo); }
	inline void WriteBind(){ glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo); }
	inline void ReadBind() { glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo); }
	
	inline void UnbindTextures(){
		for (int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	inline const std::vector<unsigned int>& GetTextures() { return textures; }
};


//FrameBuffer object
class GBuffer : public Framebuffer
{
	void Init() override;
public:
	GBuffer(int width, int height)
	{
		windowWidth = width;
		windowHeight = height;
		Init();
	};
	
	~GBuffer()
	{
		glDeleteBuffers(1, &fbo);
	}
	enum GBUFFER_TYPE
	{
		GBUFFER_TYPE_POSITION,
		GBUFFER_TYPE_NORMAL,
		GBUFFER_TYPE_ALBEDO,
		GBUFFER_NUM_TEXTURES,

	};
};

class LightBuffer : public Framebuffer
{
	void Init() override;
	
public:
	LightBuffer(int width, int height)
	{
		windowWidth = width;
		windowHeight = height;
		Init();
	}

	~LightBuffer()
	{
		glDeleteBuffers(1, &fbo);
	}


};

class Quad
{
private:
	VertexArray quadVAO;
	VertexBuffer quadVBO;
	IndexBuffer quadIBO;

public:
	Quad();
	void Draw(ShaderResource& program);
};