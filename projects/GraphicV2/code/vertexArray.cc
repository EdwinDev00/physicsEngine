#include "vertexArray.h"
#include "vertexBufferLayout.h"

#include <gl/glew.h>

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &renderID);
	glBindVertexArray(renderID);
}

VertexArray::~VertexArray()
{
}

void VertexArray::AddBuffer(const VertexBuffer& p_buffer, const VertexBufferLayout& p_layout)
{
	Bind();
	p_buffer.Bind();
	const auto& elements = p_layout.GetElement();
	unsigned int offset = 0; 
	for(int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		glEnableVertexAttribArray(i); //Doesn't matter when you enable the vertex attrib array as long as the buffer is still bound
		glVertexAttribPointer(i, element.count, element.type, 
			element.normalized , p_layout.GetStride(), (const void*)offset); // linking VAO with currently bound VBO 
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
}

void VertexArray::Bind() const
{
	glBindVertexArray(renderID);
}

void VertexArray::UnBind() const
{
	glBindVertexArray(0);
}
