#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <string>

#include <fstream>
#include <iostream>

#include <GL/glew.h>

std::string get_File_contents(const char* fileName) {
	std::string content;
	std::ifstream fileStream(fileName, std::ios::in);

	if (!fileStream.is_open()) {
		std::cerr << "Could not read file " << fileName << ". File Doesn't exist";
			return "Failed";
	}

	std::string line = "";
	while (!fileStream.eof()) //if not end of file
	{
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

class Shader {
	public:
		GLuint ID; //Reference

		void ReadShaderFile(const char* VertexFilePath, const char* FragmentFilePath);
};
#endif // !SHADER_CLASS_H


void Shader::ReadShaderFile(const char* VertexFilePath, const char* FragmentFilePath) {
	std::string vertexCode = get_File_contents(VertexFilePath);
	std::string fragmentCode = get_File_contents(FragmentFilePath);

	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();
}