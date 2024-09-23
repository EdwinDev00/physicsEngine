#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "core/math/mat4.h"
#include "core/math/vec4.h"
#include "core/math/vec3.h"

#include "shaderClass.h"

class Camera
{
	public:
		// Stores the main vectors of the camera
		vec3 Position;
		vec3 Orientation = vec3(0, 0, -1);
		vec3 Up_dir = vec3(0, 1, 0);

		// Stores the width and height of the window
		int width, height;

		// Adjust the speed of the camera and it's sensitivity when looking around
		float speed = 0.1f;
		float sensitivity = 100.0f;

		Camera();
		// Camera constructor to set up initial values
		Camera(int width, int height, vec3 position);
		void SetupCamera(int width, int height, vec3 position);

		// Updates and exports the camera matrix to the Vertex Shader
		void Matrix(float Fovdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform);
		// Handles camera inputs
		void Inputs(GLFWwindow* window);
};

#endif // !CAMERA_CLASS_H

