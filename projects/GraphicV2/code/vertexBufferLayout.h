#pragma once
#include <vector>
#include <GL/glew.h>

#define VERTEX 10

struct VertexBufferElement
{
	unsigned int type;
	unsigned int count; 
	unsigned char normalized;

	static unsigned int GetSizeOfType(unsigned int p_type)
	{
		switch(p_type)
		{
		case GL_FLOAT: return 4;
		case GL_UNSIGNED_INT: return 4;
		case GL_UNSIGNED_BYTE: return 1;
		case VERTEX: return 32;
		}

		return 0;
	}
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> elements;
	unsigned int stride; 

public:
VertexBufferLayout(): stride(0){}

	template<typename T>
	void Push(unsigned int p_count)
	{
		static_assert(false);
	}

	template<>
	void Push<float>(unsigned int p_count)
	{
		elements.push_back({ GL_FLOAT, p_count, GL_FALSE });
		stride += p_count * VertexBufferElement::GetSizeOfType(GL_FLOAT) ; //in bytes
	}

	template<>
	void Push<unsigned int>(unsigned int p_count)
	{
		elements.push_back({ GL_UNSIGNED_INT, p_count, GL_FALSE });
		stride += p_count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT); //in bytes
	}

	template<>
	void Push<unsigned char*>(unsigned int p_count)
	{
		elements.push_back({ GL_UNSIGNED_BYTE, p_count, GL_TRUE });
		stride += p_count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE); //in bytes
	}

	inline const std::vector<VertexBufferElement> GetElement() const& { return elements; }
	inline unsigned int GetStride() const { return stride; }
};