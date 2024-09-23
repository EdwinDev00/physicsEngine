#include "IndexBuffer.h"
#include <gl/glew.h>

IndexBuffer::IndexBuffer(const unsigned int* p_data, unsigned int p_count):
	indiciesCount(p_count)
{
	//ASSERT(sizeof(unsigned int) == sizeof(GLuint));

	glGenBuffers(1, &renderID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, p_count * sizeof(unsigned int), p_data, GL_STATIC_DRAW);
}

IndexBuffer::IndexBuffer(std::vector<unsigned int> p_data)
	: indiciesCount(p_data.size())
{
	glGenBuffers(1, &renderID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, p_data.size() * sizeof(unsigned int), p_data.data(), GL_STATIC_DRAW);
}


IndexBuffer::~IndexBuffer()
{
	//if(renderID != 0)
	//GLCall(glDeleteBuffers(1, &renderID));
}

void IndexBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderID);
}

void IndexBuffer::Unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::SetData(std::vector<unsigned int> p_data)
{
	indiciesCount = p_data.size();
	glGenBuffers(1, &renderID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, p_data.size() * sizeof(unsigned int), p_data.data(), GL_STATIC_DRAW);
}