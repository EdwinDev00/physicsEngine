#include "shaderResource.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <gl/glew.h>

#include "core/math/mat4.h"

ShaderResource::ShaderResource(const std::string& filePath)
	: filePath(filePath), renderID(0)
{
	ShaderProgramSource source = ParseShader(filePath);
	renderID = CreateShader(source.VertexSource, source.FramgmentSource);

}

ShaderResource::~ShaderResource()
{
	//if(renderID != 0)
	glDeleteProgram(renderID);
}

//Parse the shader
ShaderProgramSource ShaderResource::ParseShader(const std::string& fileSource)
{
	std::ifstream stream(fileSource);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2]; // [0] vertex, [1] fragment
	ShaderType type = ShaderType::NONE;

	while (std::getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos) //not equal to invalid string position
		{
			if (line.find("vertex") != std::string::npos)
				//Set to Vertex mode
				type = ShaderType::VERTEX;

			else if (line.find("fragment") != std::string::npos)
				//Set to Fragment mode
				type = ShaderType::FRAGMENT;
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}
	//Return a shaderprogram struct 
	return { ss[0].str(), ss[1].str() };
}

unsigned int ShaderResource::CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str(); //same as &source[0]
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	// ERROR HANDLING
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (!result) //Result == GL_FALSE aka if the compile was not a success 
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

		char* message = (char*) alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);

		std::cout << "Failed to compile " <<
			(type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << " Shader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

unsigned int ShaderResource::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	//Compile the shader 
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

void ShaderResource::Bind() const
{
	glUseProgram(renderID);
}
void ShaderResource::UnBind() const 
{
	glUseProgram(0);
}

void ShaderResource::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void ShaderResource::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
	glUniform3f(GetUniformLocation(name), v0,v1,v2);
}

void ShaderResource::SetUniform1f(const std::string& name, float v0)
{
	glUniform1f(GetUniformLocation(name), v0);
}

void ShaderResource::SetUniform1i(const std::string& name,int value)
{
	glUniform1i(GetUniformLocation(name), value);
}

void ShaderResource::SetUniformVec2(const std::string& name, vec2 v)
{
	glUniform2f(GetUniformLocation(name), v.x, v.y);
}

void ShaderResource::SetUniformVec3(const std::string& name, vec3 v)
{
	glUniform3f(GetUniformLocation(name), v.x, v.y, v.z);
}

void ShaderResource::SetUniformVec4(const std::string& name, vec4 v)
{
	glUniform4f(GetUniformLocation(name), v.x, v.y, v.z,v.w);
}

void ShaderResource::SetUniformMat4f(const std::string& name, const mat4& matrix)
{
	glUniformMatrix4fv(GetUniformLocation(name),1,GL_FALSE,&matrix.m[0][0]);
}

int ShaderResource::GetUniformLocation(const std::string& name) const
{
	if (uni_LocationCache.find(name) != uni_LocationCache.end())
		return uni_LocationCache[name]; //Already exist in the cache, retrieve it

	int location = glGetUniformLocation(renderID, name.c_str());

	if (location == -1) //Get -1 = Can't find the location of the uniform (can also mean the uniform is not used or has been removed )
		std::cout << "WARNING: Uniform " << filePath << " - " << name << " doesn't exist!" << std::endl;

	uni_LocationCache[name] = location;
	return location;
}