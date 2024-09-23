#ifndef MESHRESOURCE_CLASS_H
#define MESHRESOURCE_CLASS_H


#include <fstream>
#include <iostream>
#include <GL/glew.h>
#include "core/math/mat4.h"

//	Handle Reading shader file
std::string get_File_contents(const char* fileName) {
	std::string content;
	std::ifstream fileStream(fileName, std::ios::in);

	if (!fileStream.is_open()) {
		std::cerr << "Could not read file " << fileName << ". File Doesn't exist" << std::endl;
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

class MeshResource {
	public:
		~MeshResource() { 
			//invoked when closing the program
			std::cout << "Invoked destructor" << std::endl; 
			// TODO CLEAN UP MEMORY (DELETE UNNECSSARY DATA)
			glDeleteProgram(ShaderProgram);			
		}

		// Reference
		GLuint VBO,VAO,EBO;
		GLuint VertexShader;
		GLuint FragmentShader;
		GLuint ShaderProgram;
		
		//uniform ref
		GLuint  uniTransID;
		GLuint uniVTransID;

		mat4 transform;

		//Setup shaders
		GLuint SetVShader(); //Setup Vertex Shader
		GLuint SetFShader(); //Setup Fragment Shader
		GLuint SetProgramObj(); //Setup Shader Program
		
		// VBO
		void GenerateVBuffer(GLfloat* vertices, GLsizeiptr size); //Create Vbuffer Object
		
		// EBO
		void GenerateEBuffer(GLuint* indices, GLsizeiptr size);

		// VAO
		void GenerateVArray(); //Create Varray Object
		void LinkAttrib(GLuint& VBO, GLuint index, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
		
		//Bind Methods
		void BindArrayBuffer(GLuint buffer);
		void BindVArrayBuffer();

		//Unbind methods
		void UnbindVBuffer();
		void UnbindEBuffer();
		void UnbindVertexArray();

		//test
		void UnbindEverything();

		void DeleteShader(GLuint shader);
		void ReadShaderFile(const char* vertexFilePath, const char* fragmentFilePath);

		void translatePos(vec3 vpos);


	private:
		const GLchar* VertexSource;
		const GLchar* FragmentSource;
};
#endif // !MESHRESOURCE_CLASS_H

//Methods Definition




#pragma region SetShaders&Program
GLuint MeshResource::SetVShader() {
	// Create Vertex Shader Object and get its reference
	VertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Attach Vertex Shader source to the Vertex Shader Object
	glShaderSource(VertexShader, 1, &VertexSource, NULL);
	// Compile the Vertex Shader into machine code
	glCompileShader(VertexShader);
	
	return VertexShader;
}

GLuint MeshResource::SetFShader() {
	// Create Fragment Shader Object and get its reference
	FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Attach Fragment Shader source to the Fragment Shader Object
	glShaderSource(FragmentShader, 1, &FragmentSource, NULL);
	// Compile the Vertex Shader into machine code
	glCompileShader(FragmentShader);
	
	return FragmentShader;
}

GLuint MeshResource::SetProgramObj() {
	// Create Shader Program Object and get its reference
	ShaderProgram = glCreateProgram();
	// Attach the Vertex and Fragment Shaders to the Shader Program
	glAttachShader(ShaderProgram, VertexShader);
	glAttachShader(ShaderProgram, FragmentShader);
	// Wrap-up/Link all the shaders together into the Shader Program
	glLinkProgram(ShaderProgram);
	return ShaderProgram;
}

#pragma endregion


#pragma region VBO_Properties

//Generate VBO and bind it to GL_ARRAY_BUFFER
void MeshResource::GenerateVBuffer(GLfloat* vertices, GLsizeiptr size) {
	// Generate the VBO with only 1 object each
	glGenBuffers(1, &VBO);
	// Bind the VBO specifying it's a GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Introduce the vertices into the VBO
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}
#pragma endregion

#pragma region EBO_Properties
void MeshResource::GenerateEBuffer(GLuint* indices, GLsizeiptr size) {
	glGenBuffers(1, &EBO); // Generate the EBO 
	// Bind the EBO specifying it's a GL_ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// Introduce the indices into the EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}
#pragma endregion

#pragma region VAO_Properties

void MeshResource::GenerateVArray() {
	// Generate the VAO with only 1 object each
	glGenVertexArrays(1,&VAO);
}

void MeshResource::LinkAttrib(GLuint& VBO, GLuint index, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset) {
	// Vertex Buff / specified vertex attribute to be modified
	BindArrayBuffer(VBO);
	glVertexAttribPointer(index, numComponents, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(index);
	UnbindVBuffer();
}
#pragma endregion


#pragma region ReadShaderFile

void MeshResource::ReadShaderFile(const char* vertexFilePath, const char* fragmentFilePath) {

	std::string vertexCode = get_File_contents(vertexFilePath);
	std::string fragmentCode = get_File_contents(fragmentFilePath);

	VertexSource = vertexCode.c_str();
	FragmentSource = fragmentCode.c_str();

	SetVShader();
	SetFShader();
	SetProgramObj();

	DeleteShader(VertexShader);
	DeleteShader(FragmentShader);
	//return VertexSource, FragmentSource;
}
#pragma endregion

#pragma region General_Functionality

void MeshResource::BindArrayBuffer(GLuint buffer) {
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
}

void MeshResource::BindVArrayBuffer() {
	glBindVertexArray(VAO);
}

void MeshResource::UnbindVBuffer() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshResource::UnbindEBuffer() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MeshResource::UnbindVertexArray() {
	glBindVertexArray(0);
}

void MeshResource::UnbindEverything() {
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MeshResource::DeleteShader(GLuint shader) {
	glDeleteShader(shader);
}
#pragma endregion

#pragma region Mesh_Manipulation/view/projection

void MeshResource::translatePos(vec3 vpos) {
	glUniform4f(uniVTransID, vpos.x, vpos.y, vpos.z, 1);
}
#pragma endregion
