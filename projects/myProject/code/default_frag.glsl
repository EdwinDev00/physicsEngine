#version 430

// Outputs color in RGBA
out vec4 Color;

// Inputs the color from the Vertex Shader
layout(location=0) in vec4 color;

//Texture
layout(location=2) in vec2 texCoord;
uniform sampler2D tex0;

void main()
{
	//Color = color;
	//uncomment for texture
	Color = texture(tex0,texCoord);
}