#pragma once
#include <vector>

struct Vertex;

class VertexBuffer
{
private:
	unsigned int renderID = 0;//numeric id to the renderer

public:
	VertexBuffer(){}
	VertexBuffer(const void* p_data, unsigned int p_size);
	VertexBuffer(std::vector<float> p_data);
	VertexBuffer(std::vector<Vertex> p_data);


	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
	void SetData(std::vector<Vertex> p_data);

};