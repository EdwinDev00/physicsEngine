#ifndef GRAPHICSNODE_CLASS_H
#define GRAPHICSNODE_CLASS_H

#include <memory> //smart pointer

#include "MeshResource.h"
#include "ShaderResource.h"
#include "TextureResource.h"

#include "GLFW/glfw3.h"


class GraphicsNode 
{
	
public:
	GraphicsNode();

	MeshResource mesh;
	std::shared_ptr<ShaderResource> s_shader;
	TextureResource texture;

	// Transform for the renderobject (vec4 or matrix)
	mat4 transform; //renderobject transform aka model matrix

	// Render out the respective object
	void drawMesh(std::shared_ptr<GraphicsNode> obj);

	// Manipulate mesh movement 
	void translateMesh(vec3 position);
	void rotationX(); 
	void rotationY();
};
#endif // !GRAPHICSNODES_CLASS_H
