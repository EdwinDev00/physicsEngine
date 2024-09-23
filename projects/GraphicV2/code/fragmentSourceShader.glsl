#version 430
out vec4 Color;

layout(location=0) in vec4 color;
layout(location=2) in vec2 TexCoord;

//Texture uniform
uniform sampler2D tex0;

void main()
{
	//Color = color;
	Color = texture(tex0,TexCoord);
};