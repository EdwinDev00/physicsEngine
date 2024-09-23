#pragma once
#include "MeshResource.h"

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

#pragma region SetShaders&Program
GLuint MeshResource::SetVShader() {
	// Create Vertex Shader Object and get its reference
	VertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Attach Vertex Shader source to the Vertex Shader Object
	glShaderSource(VertexShader, 1, &VertexSource, NULL);
	// Compile the Vertex Shader into machine code
	glCompileShader(VertexShader);

	GLint shaderLogSize;
	glGetShaderiv(VertexShader, GL_INFO_LOG_LENGTH, &shaderLogSize);
	if (shaderLogSize > 0)
	{
		GLchar* buf = new GLchar[shaderLogSize];
		glGetShaderInfoLog(VertexShader, shaderLogSize, NULL, buf);
		printf("[VERTEX SHADER COMPILE ERROR]: %s", buf);
		delete[] buf;
	}

	return VertexShader;
}

GLuint MeshResource::SetFShader() {
	// Create Fragment Shader Object and get its reference
	FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Attach Fragment Shader source to the Fragment Shader Object
	glShaderSource(FragmentShader, 1, &FragmentSource, NULL);
	// Compile the Vertex Shader into machine code
	glCompileShader(FragmentShader);

	// get error log
	GLint shaderLogSize;
	glGetShaderiv(FragmentShader, GL_INFO_LOG_LENGTH, &shaderLogSize);
	if (shaderLogSize > 0)
	{
		GLchar* buf = new GLchar[shaderLogSize];
		glGetShaderInfoLog(FragmentShader, shaderLogSize, NULL, buf);
		printf("[FRAGMENT SHADER COMPILE ERROR]: %s", buf);
		delete[] buf;
	}

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
	DeleteShader(VertexShader);
	DeleteShader(FragmentShader);

	GLint shaderLogSize;
	glGetProgramiv(ShaderProgram, GL_INFO_LOG_LENGTH, &shaderLogSize);
	if (shaderLogSize > 0)
	{
		GLchar* buf = new GLchar[shaderLogSize];
		glGetProgramInfoLog(ShaderProgram, shaderLogSize, NULL, buf);
		printf("[PROGRAM LINK ERROR]: %s", buf);
		delete[] buf;
	}
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

//Test
//Generate VBO and bind it to GL_ARRAY_BUFFER
void MeshResource::GenerateVBuffer(std::vector<Vertex>& vertices) {

	// Generate the VBO with only 1 object each
	glGenBuffers(1, &VBO);
	// Bind the VBO specifying it's a GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Introduce the vertices into the VBO
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
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

void MeshResource::GenerateEBuffer(std::vector<GLuint>& indices) {
	glGenBuffers(1, &EBO); // Generate the EBO 
	// Bind the EBO specifying it's a GL_ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// Introduce the indices into the EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}
#pragma endregion

#pragma region VAO_Properties

void MeshResource::GenerateVArray() {
	// Generate the VAO with only 1 object each
	glGenVertexArrays(1, &VAO);
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

	DeleteShader(this->VertexShader);
	DeleteShader(this->FragmentShader);
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

//void MeshResource::Delete()
//{
//	
//}
#pragma endregion


#pragma region MeshCreations
//Pre Mesh samples
//MeshResource MeshResource::createCube(float width, float height, float depth) {
//	MeshResource newMesh;
//	//Vertices & indices Data
//	Vertex vertices[] =
//	{
//		//			Position		/			Colour				/	Texture Coord
//			//Front
//		Vertex{vec3(-width,-height, depth), vec4(1.0f, 0.0f, 0.0f, 1.0f),  0.0f, 0.0f},   // v0
//		Vertex{vec3( width,-height, depth), vec4(0.0f, 1.0f, 0.0f, 1.0f),  1.0f, 0.0f},   // v1
//		Vertex{vec3( width, height, depth), vec4(0.0f, 0.0f, 1.0f, 1.0f),  1.0f, 1.0f},   // v2
//		Vertex{vec3(-width, height, depth), vec4(1.0f, 1.0f, 1.0f, 1.0f),  0.0f, 1.0f},   // v3
//		//Back															   
//		Vertex{vec3(-width,-height,-depth), vec4(1.0f, 0.0f, 0.0f, 1.0f),  0.0f, 0.0f},   // v4
//		Vertex{vec3( width,-height,-depth), vec4(0.0f, 1.0f, 0.0f, 1.0f),  1.0f, 0.0f},   // v5
//		Vertex{vec3( width, height,-depth), vec4(0.0f, 0.0f, 1.0f, 1.0f),  1.0f, 1.0f},   // v6
//		Vertex{vec3(-width, height,-depth), vec4(1.0f, 1.0f, 1.0f, 1.0f),  0.0f, 1.0f},   // v7
//	};
//
//	GLuint indices[] =
//	{
//		 //Front
//		 0,1,2, 2,0,3,
//		 //Right
//		 1,5,6, 6,2,1,
//		 //Back
//		 7,6,5, 5,4,7,
//		 //Left
//		 4,0,3, 3,7,4,
//		 //Bottom
//		 4,5,1, 1,0,4,
//		 //Top
//		 3,2,6, 6,7,3
//	};
//
//	//Generate shaders from file
//	newMesh.ReadShaderFile("..\\projects\\Cam_texProject\\code\\vertexSourceShader.glsl"
//		, "..\\projects\\Cam_texProject\\code\\fragmentSourceShader.glsl");
//
//	//set the vertices to the vector array
//	std::vector<Vertex>v(vertices, vertices + sizeof(vertices) / sizeof(Vertex));
//	std::vector<GLuint>i(indices, indices + sizeof(indices) / sizeof(GLuint));
//
//	newMesh.MeshVertices = v;
//	newMesh.MeshIndices = i;
//
//	//Setup the mesh
//	newMesh.GenerateVArray();
//	newMesh.BindVArrayBuffer();
//
//	newMesh.GenerateVBuffer(newMesh.MeshVertices);
//
//	newMesh.GenerateEBuffer(newMesh.MeshIndices);
//
//	newMesh.LinkAttrib(newMesh.VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
//	newMesh.LinkAttrib(newMesh.VBO, 1, 4, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
//	newMesh.LinkAttrib(newMesh.VBO, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)(7 * sizeof(float)));
//
//	newMesh.UnbindEverything();
//
//
//	return newMesh;
//}

//Testing out with the destructor
MeshResource* MeshResource::createCube1(float width, float height, float depth) {
	MeshResource *newMesh = this;

	//Vertices & indices Data
	Vertex vertices[] =
	{
		//NEED TO DEFINE 36 VERTEX (Which gives me individual Face of the cube so the texture not clipping
		//			Position		/			Colour				/	Texture Coord
		////Front
		//Vertex{vec3(-width,-height, depth), vec4(1.0f, 0.0f, 0.0f, 1.0f),  0.0f, 0.0f},   // v0
		//Vertex{vec3( width,-height, depth), vec4(0.0f, 1.0f, 0.0f, 1.0f),  1.0f, 0.0f},   // v1
		//Vertex{vec3( width, height, depth), vec4(0.0f, 0.0f, 1.0f, 1.0f),  1.0f, 1.0f},   // v2
		//Vertex{vec3(-width, height, depth), vec4(1.0f, 1.0f, 1.0f, 1.0f),  0.0f, 1.0f},   // v3
		////Back															   
		//Vertex{vec3(-width,-height,-depth), vec4(1.0f, 0.0f, 0.0f, 1.0f),  0.0f, 0.0f},   // v4
		//Vertex{vec3( width,-height,-depth), vec4(0.0f, 1.0f, 0.0f, 1.0f),  1.0f, 0.0f},   // v5
		//Vertex{vec3( width, height,-depth), vec4(0.0f, 0.0f, 1.0f, 1.0f),  1.0f, 1.0f},   // v6
		//Vertex{vec3(-width, height,-depth), vec4(1.0f, 1.0f, 1.0f, 1.0f),  0.0f, 1.0f},   // v7

		//Testing (14 vertices to cover the 6 faces)
		Vertex{vec3(-width,-height,-depth), vec4(1.0f, 1.0f, 1.0f, 1.0f),  0.0f, 0.0f},   // v0
		Vertex{vec3( width,-height,-depth), vec4(1.0f, 1.0f, 1.0f, 1.0f),  1.0f, 0.0f},   // v1
		Vertex{vec3( width, height,-depth), vec4(1.0f, 1.0f, 1.0f, 1.0f),  2.0f, 0.0f},   // v2
		Vertex{vec3(-width, height,-depth), vec4(1.0f, 1.0f, 1.0f, 1.0f),  3.0f, 0.0f},   // v3
		Vertex{vec3(-width,-height,-depth), vec4(1.0f, 1.0f, 1.0f, 1.0f),  4.0f, 0.0f},   // v4

		Vertex{vec3(-width,-height, depth), vec4(1.0f, 1.0f, 1.0f, 1.0f),  0.0f, 1.0f},   // v5
		Vertex{vec3( width,-height, depth), vec4(1.0f, 1.0f, 1.0f, 1.0f),  1.0f, 1.0f},   // v6
		Vertex{vec3( width, height, depth), vec4(1.0f, 1.0f, 1.0f, 1.0f),  2.0f, 1.0f},   // v7
		Vertex{vec3(-width, height, depth), vec4(1.0f, 1.0f, 1.0f, 1.0f),  3.0f, 1.0f},   // v8
		Vertex{vec3(-width,-height, depth), vec4(1.0f, 1.0f, 1.0f, 1.0f),  4.0f, 1.0f},   // v9

		Vertex{vec3(-width, height,-depth), vec4(1.0f, 1.0f, 1.0f, 1.0f),  0.0f,-1.0f},   // v10
		Vertex{vec3( width, height,-depth), vec4(1.0f, 1.0f, 1.0f, 1.0f),  1.0f,-1.0f},   // v11

		Vertex{vec3(-width, height, depth), vec4(1.0f, 1.0f, 1.0f, 1.0f),  0.0f, 2.0f},   // v12
		Vertex{vec3( width, height, depth), vec4(1.0f, 1.0f, 1.0f, 1.0f),  1.0f, 2.0f},   // v13
	};

	GLuint indices[] =
	{
		////Front
		//0,1,2, 2,0,3,
		////Right
		//1,5,6, 6,2,1,
		////Back
		//7,6,5, 5,4,7,
		////Left
		//4,0,3, 3,7,4,
		////Bottom
		//4,5,1, 1,0,4,
		////Top
		//3,2,6, 6,7,3
		
		//Test
		 0, 1, 5,  5, 1, 6,
		 1, 2, 6,  6, 2, 7,
		 2, 3, 7,  7, 3, 8,
		 3, 4, 8,  8, 4, 9,
		10,11, 0,  0,11, 1,
		 5, 6,12, 12, 6,13
		
	};

	//Generate shaders from file
	newMesh->ReadShaderFile("..\\projects\\Cam_texProject\\code\\vertexSourceShader.glsl"
		, "..\\projects\\Cam_texProject\\code\\fragmentSourceShader.glsl");

	//set the vertices to the vector array
	std::vector<Vertex>v(vertices, vertices + sizeof(vertices) / sizeof(Vertex));
	std::vector<GLuint>i(indices, indices + sizeof(indices) / sizeof(GLuint));

	newMesh->MeshVertices = v;
	newMesh->MeshIndices = i;

	//Setup the mesh
	newMesh->GenerateVArray();
	newMesh->BindVArrayBuffer();

	newMesh->GenerateVBuffer(newMesh->MeshVertices);

	newMesh->GenerateEBuffer(newMesh->MeshIndices);

	newMesh->LinkAttrib(newMesh->VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	newMesh->LinkAttrib(newMesh->VBO, 1, 4, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	newMesh->LinkAttrib(newMesh->VBO, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)(7 * sizeof(float)));

	newMesh->UnbindEverything();


	return newMesh;
}

//Create own Meshes by passing vertices & indices (using Vertex struct)
MeshResource MeshResource::OCMesh(std::vector<Vertex>& inVertices, std::vector<GLuint>& inIndices) {

	MeshResource newMesh; //Create newMesh

	newMesh.ReadShaderFile("..\\projects\\Cam_texProject\\code\\vertexSourceShader.glsl"
		, "..\\projects\\Cam_texProject\\code\\fragmentSourceShader.glsl");

	newMesh.MeshVertices = inVertices;
	newMesh.MeshIndices = inIndices;

	//Setup the mesh 
	newMesh.GenerateVArray();
	newMesh.BindVArrayBuffer();

	newMesh.GenerateVBuffer(inVertices);

	newMesh.GenerateEBuffer(inIndices);

	newMesh.LinkAttrib(newMesh.VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	newMesh.LinkAttrib(newMesh.VBO, 1, 4, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	newMesh.LinkAttrib(newMesh.VBO, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)(7 * sizeof(float)));

	newMesh.UnbindEverything();

	return newMesh; //returns the custom made mesh
}

#pragma endregion


