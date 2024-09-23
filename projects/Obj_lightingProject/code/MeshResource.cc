#pragma once

#include "MeshResource.h"

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


#pragma region General_Functionality

#pragma region Binds & Unbinds

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
#pragma endregion


#pragma endregion


#pragma region MeshCreations
//Pre Mesh samples
MeshResource MeshResource::createCube(float width, float height, float depth) {
	MeshResource newMesh;
	//Vertices & indices Data
	Vertex vertices[] =
	{
		//			Position		/			Colour				/		Texture Coord		/		Normal
			//Front
		Vertex{vec3(-width,-height, depth), vec4(1.0f, 0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)},   // v0
		Vertex{vec3( width,-height, depth), vec4(0.0f, 1.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)},   // v1
		Vertex{vec3( width, height, depth), vec4(0.0f, 0.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)},   // v2
		Vertex{vec3(-width, height, depth), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)},   // v3
		//Back															  	   
		Vertex{vec3(-width,-height,-depth), vec4(1.0f, 0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)},   // v4
		Vertex{vec3(width,-height,-depth), vec4(0.0f, 1.0f, 0.0f, 1.0f),  vec2(1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)},   // v5
		Vertex{vec3( width, height,-depth), vec4(0.0f, 0.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)},   // v6
		Vertex{vec3(-width, height,-depth), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)},   // v7
	};

	GLuint indices[] =
	{
		 //Front
		 0,1,2, 2,0,3,
		 //Right
		 1,5,6, 6,2,1,
		 //Back
		 7,6,5, 5,4,7,
		 //Left
		 4,0,3, 3,7,4,
		 //Bottom
		 4,5,1, 1,0,4,
		 //Top
		 3,2,6, 6,7,3
	};

	//set the vertices to the vector array
	std::vector<Vertex>v(vertices, vertices + sizeof(vertices) / sizeof(Vertex));
	std::vector<GLuint>i(indices, indices + sizeof(indices) / sizeof(GLuint));

	newMesh.MeshVertices = v;
	newMesh.MeshIndices = i;

	//Setup the mesh // move this to a draw call later on
	newMesh.GenerateVArray();
	newMesh.BindVArrayBuffer();

	newMesh.GenerateVBuffer(newMesh.MeshVertices);

	newMesh.GenerateEBuffer(newMesh.MeshIndices);

	newMesh.LinkAttrib(newMesh.VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	newMesh.LinkAttrib(newMesh.VBO, 1, 4, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	newMesh.LinkAttrib(newMesh.VBO, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)(7 * sizeof(float)));
	newMesh.LinkAttrib(newMesh.VBO, 3, 3, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));


	newMesh.UnbindEverything();

	return newMesh;
}

//Create own Meshes by passing vertices & indices (using Vertex struct)
MeshResource MeshResource::OCMesh(std::vector<Vertex>& inVertices, std::vector<GLuint>& inIndices) {

	MeshResource newMesh; //Create newMesh

	newMesh.MeshVertices = inVertices;
	newMesh.MeshIndices = inIndices;

	//Setup the mesh //Probally move this to a draw call later
	newMesh.GenerateVArray();
	newMesh.BindVArrayBuffer();

	newMesh.GenerateVBuffer(inVertices);

	newMesh.GenerateEBuffer(inIndices);

	newMesh.LinkAttrib(newMesh.VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	newMesh.LinkAttrib(newMesh.VBO, 1, 4, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	newMesh.LinkAttrib(newMesh.VBO, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)(7 * sizeof(float)));
	newMesh.LinkAttrib(newMesh.VBO, 3, 3, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));

	newMesh.UnbindEverything();

	return newMesh; //returns the custom made mesh
}


#pragma endregion

#pragma region Obj_Func

//Works now
std::vector<Vertex> MeshResource::readObj1(const char* fileName)
{

	//Create local variable to store
	std::vector<vec3> Obj_vertex_positions;
	std::vector<vec3> Obj_normals;
	std::vector<vec2> Obj_textureCoord;


	//Obj vertex Array		
	std::vector<Vertex> vertices;

	//face vectors
	std::vector<int> vertex_position_indicies; //vertex
	std::vector<int> vertex_texcoord_indicies; // texture
	std::vector<int> vertex_normal_indicies; // normal


	//temp
	vec3 temp_vec3;
	vec2 temp_vec2;
	int temp_int = 0;

	//File
	std::stringstream ss;
	std::ifstream in_file(fileName);
	std::string line = "";
	std::string prefix = "";

	// Error: File open error
	if (!in_file.is_open())
	{
		std::cerr << "Could not read file " << fileName << ". File does not exist.\n";
	}

	// Read one line at a time
	while (std::getline(in_file,line))
	{
		// Get the prefix of the line
		ss.clear();
		ss.str(line);
		ss >> prefix;

		if(prefix == "#")
		{
			//TODO: SKIP THE WHOLE LINE 
		}
		// Check for vertices
		else if (prefix == "v") //Vertex position
		{
			ss >> temp_vec3.x; ss >> temp_vec3.y; ss >> temp_vec3.z;
			Obj_vertex_positions.push_back(temp_vec3);
		}
		// Check for vertex normal
		else if (prefix == "vn")
		{
			ss >> temp_vec3.x; ss >> temp_vec3.y; ss >> temp_vec3.z;
			Obj_normals.push_back(temp_vec3);
		}
		// Check for texture coordinates
		else if (prefix == "vt")
		{
			ss >> temp_vec2.x; ss >> temp_vec2.y;
			Obj_textureCoord.push_back(temp_vec2);
		}
		//-1 important
		//check for faces
		else if (prefix == "f") // = Indices
		{
			int counter = 0;
			while(ss >> temp_int)
			{
				//Pushing indices into correct array
				if (counter == 0)
				{
					vertex_position_indicies.push_back(temp_int);
				}
				else if (counter == 1)
				{
					vertex_texcoord_indicies.push_back(temp_int);
				}
				else if (counter == 2)
				{
					vertex_normal_indicies.push_back(temp_int);
				}
				
				//Handle characters
				if(ss.peek() == '/')
				{
					++counter;
					ss.ignore(1, '/');
				}

				else if(ss.peek() == ' ')
				{
					++counter;
					ss.ignore(1, ' ');
				}

				//Check if this resets
				//Reset the counter
				if (counter > 2)
					counter = 0;
			}
		}
		else { std::cout << "Non matched prefix" << std::endl; }
	}
		
		// Build final vertex array (mesh)
		//objvertex size = exactly size as all three of the vertex_indicies
	   // TODO: DISCOVERY WHERE THE FACES (INDICIES) ARE STORED? (STORE THOSE INDICES IN A VARIABLE)
		vertices.resize(vertex_position_indicies.size(), Vertex()); //reserve space to how many vertices we have
		// Load in all indices
		for (int i = 0; i < vertices.size(); ++i)
		{
			//Format: vertex/Texture/Normal obj
			unsigned int vertexIndex = vertex_position_indicies[i];
			unsigned int textureIndex = vertex_texcoord_indicies[i];
			unsigned int normalIndex = vertex_normal_indicies[i];

			vertices[i].vPositon = Obj_vertex_positions[vertexIndex - 1];
			vertices[i].vColour = vec4(1.f, 1.f, 1.f, 1.f); //setup some colour
			vertices[i].textureCoord = Obj_textureCoord[textureIndex - 1];
			vertices[i].vNormal = Obj_normals[normalIndex - 1]; 
			//std::cout << i << "  ";
		}
		std::cerr <<  " NR: " << vertices.size() << std::endl;


	//delete the unused vectors
	
	// Return the indices also (store them in a arrays)
	
	// Successful Load OBJ
	return vertices;
}


void MeshResource::initObj()
{
	GenerateVArray();
	BindVArrayBuffer();

	GenerateVBuffer(this->testobjVertex);

	LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	LinkAttrib(VBO, 1, 4, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	LinkAttrib(VBO, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)(7 * sizeof(float)));
	LinkAttrib(VBO, 3, 3, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));

	//glBufferData(GL_ARRAY_BUFFER, this->testobjVertex.size() * sizeof(Vertex), this->testobjVertex.data(), GL_STATIC_DRAW);

	UnbindEverything();

	//set the vertices to the vector array
	/*std::vector<Vertex>v(obj, vertices + sizeof(vertices) / sizeof(Vertex));
	std::vector<GLuint>i(indices, indices + sizeof(indices) / sizeof(GLuint));*/
	
	//MeshResource newMesh; //Create newMesh

	//newMesh.MeshVertices = inVertices;
	//newMesh.MeshIndices = inIndices;

	////Setup the mesh //Probally move this to a draw call later
	//newMesh.GenerateVArray();
	//newMesh.BindVArrayBuffer();

	//newMesh.GenerateVBuffer(inVertices);

	//newMesh.GenerateEBuffer(inIndices);

	//newMesh.LinkAttrib(newMesh.VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	//newMesh.LinkAttrib(newMesh.VBO, 1, 4, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	//newMesh.LinkAttrib(newMesh.VBO, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)(7 * sizeof(float)));
	//newMesh.LinkAttrib(newMesh.VBO, 3, 3, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));

	//newMesh.UnbindEverything();

	//return newMesh; //returns the custom made mesh
}



#pragma endregion

