#include "Camera.h"

Camera::Camera(int width, int height, vec3 position)
{
	this->width = width;
	this->height = height;
	this->Position = position;
}

void Camera::Orbit(MeshResource* mesh)
{
	// Virtual Camera Orbit around the mesh in world space
	// Initalizes matrices so they are not null Matrix
	mat4 model = mat4();
	// Continous circulation around the camera axis
	float camZ = sin(glfwGetTime()) * 5;
	float camX = cos(glfwGetTime()) * 5;

	// Assigns different transformations to each matrix
	//model = translation(vec3(0, 0.25f, 0));
	Camera::proj = perspective(50.0f, (float)(width / height), 0.1f, 100.f);
	Camera::view = lookat(vec3(camX, 1, camZ), vec3(0, 0, 0), vec3(0, 1, 0));

	// Outputs the matrices into the Vertex Shader
	//shaderProgram.setMat4Uniform("model", &model[0][0]);
	/*shader->setMat4Uniform("view", &view[0][0]);
	shader->setMat4Uniform("proj", &proj[0][0]);*/
	
	Camera::projview = proj * view;
	int modelLoc = glGetUniformLocation(mesh->ShaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

	int viewLoc = glGetUniformLocation(mesh->ShaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

	int projLoc = glGetUniformLocation(mesh->ShaderProgram, "proj");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, &proj[0][0]);
}

//void Camera::Lookat(float Fovdeg, float nearPlane, float farPlane, std::shared_ptr<ShaderResource> shader) // ShaderResource& shaderProgram
//{
//	// Makes camera look in the right direction from the right direction
//	view = lookat(Position, Position + Orientation, Up_dir);
//	// Adds Perspective projection to the scene
//	proj = perspective(Fovdeg, (float)(width / height), nearPlane, farPlane);
//
//	Camera::projview = proj * view;
//	shader->setMat4Uniform("view", &view[0][0]);
//	shader->setMat4Uniform("proj", &proj[0][0]);
//}
//
//void Camera::Inputs(int32_t key)
//{
//	//Movement WASD
//	if (key == GLFW_KEY_W) Position += Orientation * speed;
//	if (key == GLFW_KEY_A) Position += -normalize(cross(Orientation, Up_dir)) * speed;
//	if (key == GLFW_KEY_S) Position += -Orientation * speed;
//	if (key == GLFW_KEY_D) Position += normalize(cross(Orientation, Up_dir)) * speed;
//
//	// UP & DOWN
//	if (key == GLFW_KEY_SPACE) Position += Up_dir * speed;
//	if (key == GLFW_KEY_LEFT_CONTROL) Position += -Up_dir * speed;
//}
