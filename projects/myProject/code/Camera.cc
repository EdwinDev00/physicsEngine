#include "Camera.h"

Camera::Camera(int width, int height, vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Camera::Position = position;
}

void Camera::SetupCamera(int width, int height, vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Camera::Position = position;
}


void Camera::Matrix(float Fovdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform)
{
	// Initializes matrices since otherwise they will be the null matrix
	mat4 view = mat4();
	mat4 projection = mat4();

	// Makes camera look in the right direction from the right position
	view = lookat(Position, Position + Orientation, Up_dir);
	// Adds perspective to the scene
	projection = perspective(Fovdeg, (float)(width / height), nearPlane, farPlane);

	// Exports the camera matrix to the Vertex Shader
	mat4 projView = projection * view;
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, &projView[0][0]);
}

void Camera::Inputs(GLFWwindow* window)
{
	//Only for training purposes
	// Handles key inputs
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		Position += Orientation * speed;

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		Position += -normalize(cross(Orientation,Up_dir)) * speed;	
	
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		Position += -Orientation * speed;	
	
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		Position += normalize(cross(Orientation, Up_dir)) * speed;

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		Position += Up_dir * speed;

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		Position += -Up_dir * speed;

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)	{ speed = 0.4f; }
	else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE) { speed = 0.1f; }

	//Mouse movement //Dosn't work but the gist of mouse movement
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	 //Stores the coordinates of the cursor
		double mouseX;
		double mouseY;
	 //Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);

	// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
	// and then "transforms" them into degrees 
		float rotX = sensitivity * (float)(mouseY - (height / 2)) / 2;
		float rotY = sensitivity * (float)(mouseX - (height / 2)) / 2;

	 //Calculates upcoming vertical change in the Orientation
		//vec3 newOrientation = rotationaxis(normalize(cross(Orientation, Up_dir)), -rotX); //should return a vec3 instead of mat4
		//mat4 newOrientation = rotationaxis(normalize(cross(Orientation, Up_dir)), -rotY);
	
	// Decides whether or not the next vertical Orientation is legal or not
	/*	if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			Orientation = newOrientation;
		}*/

	// Rotates the Orientation left and right
		/*Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);*/
	
	// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		glfwSetCursorPos(window, (width / 2), (height / 2));
	}
	else if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

}