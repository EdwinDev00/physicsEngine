#include "gBuffer.h"

#include<iostream>
#include "dataCollection.h"

#include "vertexBufferLayout.h"
#include "shaderResource.h"

void GBuffer::Init()
{
	textures.resize(GBUFFER_NUM_TEXTURES);
	attachments.resize(GBUFFER_NUM_TEXTURES);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// - position color buffer
	glGenTextures(1, &textures[GBUFFER_TYPE_POSITION]);
	glBindTexture(GL_TEXTURE_2D, textures[GBUFFER_TYPE_POSITION]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[GBUFFER_TYPE_POSITION], 0);
	
	// - normal color buffer
	glGenTextures(1, &textures[GBUFFER_TYPE_NORMAL]);
	glBindTexture(GL_TEXTURE_2D, textures[GBUFFER_TYPE_NORMAL]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, textures[GBUFFER_TYPE_NORMAL], 0);

	// - color + specular color buffer
	glGenTextures(1, &textures[GBUFFER_TYPE_ALBEDO]);
	glBindTexture(GL_TEXTURE_2D, textures[GBUFFER_TYPE_ALBEDO]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, textures[GBUFFER_TYPE_ALBEDO], 0);

	// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	attachments = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, &attachments[0]);

	//Depth
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);


	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "FRAMEBUFFER ERROR - " << status << std::endl;
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
}


Quad::Quad()
{
	std::vector<Vertex> vertex(4);
	VertexBufferLayout layout;
	
	vertex[0].position = vec3(-1, -1, 0); 
	vertex[0].UV = vec2(0, 0);
	vertex[1].position = vec3(1, -1, 0);
	vertex[1].UV = vec2(1, 0);
	vertex[2].position = vec3(1, 1, 0);
	vertex[2].UV = vec2(1, 1);
	vertex[3].position = vec3(-1, 1, 0);
	vertex[3].UV = vec2(0, 1);

	std::vector<unsigned int>  indices =
	{
		0, 1, 2,
		2, 3, 0
	};
	
	quadVAO.Bind();
	quadVBO.SetData(vertex);
	quadIBO.SetData(indices);

	layout.Push<float>(3);
	layout.Push<float>(2);
	layout.Push<float>(3);
	layout.Push<float>(4);
	quadVAO.AddBuffer(quadVBO, layout);
}

void Quad::Draw(ShaderResource& program)
{
	program.Bind();
	quadVAO.Bind();
	quadIBO.Bind();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void LightBuffer::Init()
{
	textures.resize(1);
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &textures[0]);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[0], 0);

	attachments = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, &attachments[0]);
}
