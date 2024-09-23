#pragma once
#include <vector>
class IndexBuffer
{
private:
	unsigned int renderID = 0;//numeric id to the renderer
	unsigned int indiciesCount = 0; 
public:
	//size = amount of bytes, count = amount of element
	IndexBuffer(){}
	IndexBuffer(const unsigned int* p_data, unsigned int p_count);
	IndexBuffer(std::vector<unsigned int> data);
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;
	void SetData(std::vector<unsigned int> p_data);

	inline unsigned int GetCount() const { return indiciesCount; }

};