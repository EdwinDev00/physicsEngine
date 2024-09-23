#include "ShaderResource.h"

//Reads a text file and outputs a string with everything in the text file
std::string get_file_contents(const char* fileName)
{
	std::string content;
	std::ifstream fileStream(fileName, std::ios::in);

	if (!fileStream.is_open()) {
		std::cerr << "Could not read file " << fileName << ". File does not exist." << std::endl;
		return "";
	}

	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

void ShaderResource::GetShaderResource(const char* vertexFPath, const char* fragmentFPath)
{
	std::string vertexCode = get_file_contents(vertexFPath); // Store file vertex data to var
	std::string fragmentCode = get_file_contents(fragmentFPath); // Store file fragment data to var

	// Convert the vertex data to store on to a character array
	const char* vertexSource = vertexCode.c_str();
	// Convert the fragment data to store on to a character array 
	const char* fragmentSource = fragmentCode.c_str();

	//Vertex Shader
		// Create Vertex Shader and get its Reference
	GLuint VertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Attach Vertex Shader source to Vertex Shader object
	glShaderSource(VertexShader, 1, &vertexSource, NULL);
	// Compile the Vertex Shader into machine code
	glCompileShader(VertexShader);

	// get error log
	GLint shaderLogSize;
	glGetShaderiv(VertexShader, GL_INFO_LOG_LENGTH, &shaderLogSize);
	if(shaderLogSize > 0)
	{
		GLchar* buf = new GLchar[shaderLogSize];
		glGetShaderInfoLog(VertexShader, shaderLogSize, NULL, buf);
		printf("[VERTEX SHADER COMPILE ERROR]: %s", buf);
		delete[] buf;
	}

	//Fragment Shader
		// Create Fragment Shader and get its reference
	GLuint FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Attach Vertex Shader source to Fragment shader object
	glShaderSource(FragmentShader, 1, &fragmentSource, NULL);
	// Compile the Fragment Shader into the machine code
	glCompileShader(FragmentShader);

	// get error log
	shaderLogSize;
	glGetShaderiv(FragmentShader, GL_INFO_LOG_LENGTH, &shaderLogSize);
	if (shaderLogSize > 0)
	{
		GLchar* buf = new GLchar[shaderLogSize];
		glGetShaderInfoLog(FragmentShader, shaderLogSize, NULL, buf);
		printf("[FRAGMENT SHADER COMPILE ERROR]: %s", buf);
		delete[] buf;
	}

	//Shader program
		// Create Shader Program object and get its reference
	ID = glCreateProgram();
	// Attach the Vertex & Fragment Shader to the Shader Program
	glAttachShader(ID, VertexShader);
	glAttachShader(ID, FragmentShader);
	// Wrap-up/Link all the shaders together into the Shader program
	glLinkProgram(ID);

	// get error log
	glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &shaderLogSize);
	if(shaderLogSize > 0)
	{
		GLchar* buf = new GLchar[shaderLogSize];
		glGetProgramInfoLog(ID, shaderLogSize, NULL, buf);
		printf("[PROGRAM LINK ERROR]: %s", buf);
		delete[] buf;
	}

	// Delete the now useless Vertex & Fragment Shader objects
	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);
}

// Set uniform value
void ShaderResource::setMat4Uniform(const char* uniform, float* value)
{
	glUniformMatrix4fv(glGetUniformLocation(ID,uniform),1,GL_FALSE,value);		
}

void ShaderResource::setVec4Uniform(const char* uniform, float* value)
{
	glUniform4fv(glGetUniformLocation(ID, uniform), 1, value);
}


// Activates the Shader Program
void ShaderResource::Activate()
{
	glUseProgram(ID);
}

// Deletes the Shader Program
void ShaderResource::Delete()
{
	glDeleteProgram(ID);
}
