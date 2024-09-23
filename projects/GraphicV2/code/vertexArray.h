#pragma once

#include "vertexBuffer.h"

class VertexBufferLayout;
struct Vertex;

class VertexArray
{
private:
	unsigned int renderID; 
public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& p_buffer, const VertexBufferLayout& p_layout);

	void Bind() const;
	void UnBind() const;
};