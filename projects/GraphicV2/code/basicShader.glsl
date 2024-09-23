#shader vertex
#version 430 core

layout(location = 0) in vec3 inPosition;

uniform mat4 projView;
uniform mat4 model;

void main()
{
    gl_Position = projView * model * vec4(inPosition, 1.0);
}

#shader fragment
#version 430 core

uniform vec4 color;

out vec4 FragColor;

void main()
{
    FragColor = color;
}