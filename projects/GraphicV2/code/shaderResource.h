#pragma once
#include <string>
#include <unordered_map> 
#include <memory>
#include <iostream> //debug purpose

#include "config.h"

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FramgmentSource;
};

class ShaderResource
{
public:
	ShaderResource() {}
	ShaderResource(const std::string& filePath);
	~ShaderResource();

	void Bind() const;
	void UnBind() const;

	//Uniform Setter 
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniform3f(const std::string& name, float v0, float v1, float v2);
	void SetUniform1f(const std::string& name, float v0);

	void SetUniform1i(const std::string& name, int value);
	void SetUniformVec2(const std::string& name, glm::vec2 v);
	void SetUniformVec3(const std::string& name, glm::vec3 v);

	void SetUniformVec4(const std::string& name, glm::vec4 v);
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);


private:
	std::string filePath; //Debug only
	unsigned int renderID;
	
	//caching system for uniform //faster to get uniform //modifiable variable
	mutable std::unordered_map<std::string, int> uni_LocationCache;

	ShaderProgramSource ParseShader(const std::string& fileSource);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	
	int GetUniformLocation(const std::string& name) const;
};