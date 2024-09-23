#version 430
//Position/Coordinates
layout(location=0) in vec3 pos;

//Colors
layout(location=1) in vec4 color;

//Texture
layout(location=2) in vec2 aTex;


//Output the color for the Fragment shader
layout(location=0) out vec4 Color;

//Output the texture coordinate to the fragment shader
layout(location=2) out vec2 texCoord;


//Camera
//camMatrix = view, proj matrix (maybe model also)
// Imports the camera matrix from the main function
uniform mat4 camMatrix; 

//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 proj;


void main()
{
	//gl_Position = vec4(pos, 1);
	gl_Position = camMatrix * vec4(pos, 1);
	Color = color;
	//Assigns the texture corrdinates from the Vertex Data to "texCoord"
	texCoord = aTex;
}