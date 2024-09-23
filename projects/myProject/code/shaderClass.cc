#include "shaderClass.h"

#pragma region ReadFile

////File Reading Function
// // Reads a text file and outputs a string with everything in the text file
//std::string get_file_contents(const char* fileName) {
//	// Binary mode: Data read in from a binary stream always equal the data that were earlier written out to that stream
//	/*What is ios :: binary in C++?
//	ios::binary makes sure the data is read or written without translating new line characters to and from \r\n on the fly*/
//	std::ifstream in(fileName, std::ios::binary); //opens in binary mode, 
//
//	if (in) //open for reading
//	{ 
//		std::string contents; // store the content to the variable
//		//std::istream::seekg(offset, way), offset = relative to the way param, way = start positon of the stream 
//		in.seekg(0, std::ios::end); //Sets the position of the next character to be extracted from the input stream
//		contents.resize(in.tellg()); //Returns the position of the current character in the input stream
//		in.seekg(0, std::ios::beg);
//		in.read(&contents[0], contents.size()); // Extracts n character from the stream and stores them in the array pointed to by s
//		in.close(); // close the file
//		return (contents);
//	}
//	throw(errno); //Failed to read file throw error
//}
#pragma endregion


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

#pragma region Works to
//Shader::Shader(const char* vertexFile, const char* fragmentFile)
//{
//	// Read vertexFile and fragmentFile and store the strings
//	std::string vertexCode = get_file_contents(vertexFile); // Store file vertex data to var
//	std::string fragmentCode = get_file_contents(fragmentFile); // Store file fragment data to var
//
//	const char* vertexSource = vertexCode.c_str(); // Convert the vertex data to store on to a character array
//	const char* fragmentSource = fragmentCode.c_str(); // Convert the fragment data to store on to a character array 
//
//	//Vertex Shader
//		// Create Vertex Shader and get its Reference
//	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
//	// Attach Vertex Shader source to Vertex Shader object
//	glShaderSource(vertexShader, 1, &vertexSource, NULL);
//	// Compile the Vertex Shader into machine code
//	glCompileShader(vertexShader);
//
//	//Fragment Shader
//	// Create Fragment Shader and get its reference
//	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//	// Attach Vertex Shader source to Fragment shader object
//	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
//	// Compile the Fragment Shader into the machine code
//	glCompileShader(fragmentShader);
//
//	//Shader program
//	// Create Shader Program object and get its reference
//	ID = glCreateProgram();
//	// Attach the Vertex & Fragment Shader to the Shader Program
//	glAttachShader(ID, vertexShader);
//	glAttachShader(ID, fragmentShader);
//	// Wrap-up/Link all the shaders together into the Shader program
//	glLinkProgram(ID);
//
//	// Delete the now useless Vertex & Fragment Shader objects
//	glDeleteShader(vertexShader);
//	glDeleteShader(fragmentShader);
//}
#pragma endregion


void Shader::LoadShaderFile(const char* vertexFile, const char* fragmentFile)
{
	// Read vertexFile and fragmentFile and store the strings
	std::string vertexCode = get_file_contents(vertexFile); // Store file vertex data to var
	std::string fragmentCode = get_file_contents(fragmentFile); // Store file fragment data to var

	const char* vertexSource = vertexCode.c_str(); // Convert the vertex data to store on to a character array
	const char* fragmentSource = fragmentCode.c_str(); // Convert the fragment data to store on to a character array 

	//Vertex Shader
		// Create Vertex Shader and get its Reference
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Attach Vertex Shader source to Vertex Shader object
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	// Compile the Vertex Shader into machine code
	glCompileShader(vertexShader);

	// get error log
	GLint shaderLogSize;
	glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &shaderLogSize);
	if (shaderLogSize > 0)
	{
		GLchar* buf = new GLchar[shaderLogSize];
		glGetShaderInfoLog(vertexShader, shaderLogSize, NULL, buf);
		printf("[VERTEX SHADER COMPILE ERROR]: %s", buf);
		delete[] buf;
	}

	//Fragment Shader
	// Create Fragment Shader and get its reference
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Attach Vertex Shader source to Fragment shader object
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	// Compile the Fragment Shader into the machine code
	glCompileShader(fragmentShader);

	// get error log
	shaderLogSize;
	glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &shaderLogSize);
	if (shaderLogSize > 0)
	{
		GLchar* buf = new GLchar[shaderLogSize];
		glGetShaderInfoLog(fragmentShader, shaderLogSize, NULL, buf);
		printf("[FRAGMENT SHADER COMPILE ERROR]: %s", buf);
		delete[] buf;
	}

	//Shader program
	// Create Shader Program object and get its reference
	ID = glCreateProgram();
	// Attach the Vertex & Fragment Shader to the Shader Program
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	// Wrap-up/Link all the shaders together into the Shader program
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &shaderLogSize);
	if (shaderLogSize > 0)
	{
		GLchar* buf = new GLchar[shaderLogSize];
		glGetProgramInfoLog(ID, shaderLogSize, NULL, buf);
		printf("[PROGRAM LINK ERROR]: %s", buf);
		delete[] buf;
	}

	//// Delete the now useless Vertex & Fragment Shader objects
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}
// Activates the Shader Program
void Shader::Activate() {
	glUseProgram(ID);
}

// Deletes the Shader Program
void Shader::Delete() {
	glDeleteProgram(ID);
}