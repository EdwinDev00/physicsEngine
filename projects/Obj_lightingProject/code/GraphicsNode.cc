#include "GraphicsNode.h"

GraphicsNode::GraphicsNode()
{
	GraphicsNode::mesh = this->mesh;
	GraphicsNode::s_shader = this->s_shader;
	GraphicsNode::texture = this->texture;
	GraphicsNode::transform = this->transform;
}

void GraphicsNode::drawMesh(std::shared_ptr<GraphicsNode> obj)
{
	// Tell OpenGL which Shader Program we want to use
	obj->s_shader->Activate();
	// Apply the transform to the reletive obj
	//obj->transform = translation(transformPos);
	obj->s_shader->setMat4Uniform("model", &obj->transform[0][0]);
	// Texture
	obj->texture.Bind();
	// Bind the VAO so OpenGL knows to use it
	obj->mesh.BindVArrayBuffer();
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //if i want to see polygons
	glDrawElements(GL_TRIANGLES, obj->mesh.MeshIndices.size(), GL_UNSIGNED_INT, 0);
}

void GraphicsNode::drawObj(std::shared_ptr<GraphicsNode> obj)
{
	//// Tell OpenGL which Shader Program we want to use
	obj->s_shader->Activate();
	// Apply the transform to the reletive obj
	obj->s_shader->setMat4Uniform("model", &obj->transform[0][0]);
	//// Texture
	//obj->texture.Bind();
	//// Bind the VAO so OpenGL knows to use it
	//obj->mesh.BindVArrayBuffer();
	////glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //if i want to see polygons
	//glDrawElements(GL_TRIANGLES, obj->mesh.testobjVertex.size(), GL_UNSIGNED_INT, obj->mesh.testobjVertex.data()); // Find the indices are stored
}

void GraphicsNode::translateMesh(vec3 position)
{
	this->transform = translate(position);
}

void GraphicsNode::rotationX() // Rotate mesh on the x-axis
{
	this->transform = transform * rotationx(0.1f);
}

void::GraphicsNode::rotationY() // Rotate mesh on the y-axis
{
	this->transform = transform * rotationy(0.1f);
}