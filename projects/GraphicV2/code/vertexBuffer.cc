#include "vertexBuffer.h"
#include <gl/glew.h>
#include "dataCollection.h"


VertexBuffer::VertexBuffer(const void* p_data, unsigned int p_size)
{
	glGenBuffers(1, &renderID);
	glBindBuffer(GL_ARRAY_BUFFER, renderID);
	glBufferData(GL_ARRAY_BUFFER, p_size, p_data, GL_STATIC_DRAW);
}

VertexBuffer::VertexBuffer(std::vector<float> p_data)
{
	glGenBuffers(1, &renderID);
	glBindBuffer(GL_ARRAY_BUFFER, renderID);
	glBufferData(GL_ARRAY_BUFFER, p_data.size() * sizeof(float), p_data.data(), GL_STATIC_DRAW);
}

VertexBuffer::VertexBuffer(std::vector<Vertex> p_data)
{
	glGenBuffers(1, &renderID);
	glBindBuffer(GL_ARRAY_BUFFER, renderID);
	glBufferData(GL_ARRAY_BUFFER, p_data.size() * sizeof(Vertex), p_data.data(), GL_STATIC_DRAW);
}


VertexBuffer::~VertexBuffer()
{
}

void VertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, renderID);
}

void VertexBuffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::SetData(std::vector<Vertex> p_data)
{
	glGenBuffers(1, &renderID);
	glBindBuffer(GL_ARRAY_BUFFER, renderID);
	glBufferData(GL_ARRAY_BUFFER, p_data.size() * sizeof(Vertex), p_data.data(), GL_STATIC_DRAW);
}
