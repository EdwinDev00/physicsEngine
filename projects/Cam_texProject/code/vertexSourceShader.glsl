#version 430
layout(location=0) in vec3 pos;
layout(location=1) in vec4 color;
layout(location=2) in vec2 aTex;


layout(location=0) out vec4 Color;
layout(location=2) out vec2 TexCoord;

//Model/camera view
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;


void main()
{
	gl_Position = proj * view * model * vec4(pos, 1) ;
	Color = color;
	TexCoord = aTex;
};