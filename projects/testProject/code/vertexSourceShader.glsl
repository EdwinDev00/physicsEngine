#version 430
layout(location=0) in vec3 pos;
layout(location=1) in vec4 color;
layout(location=0) out vec4 Color;

uniform mat4 transform;
uniform vec4 transformVec;

void main()
{
	gl_Position =  transform * vec4(pos, 1) + transformVec;
	Color = color;
};