#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H 

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "core/math/mat4.h"
#include "core/math/vec4.h"
#include "core/math/vec3.h"

#include "MeshResource.h"
//#include "ShaderResource.h"
//
class Camera
{
public: 
	// Stores the main vectors of the camera
	vec3 Position;
	vec3 Orientation = vec3(0, 0, -1);
	vec3 Up_dir = vec3(0, 1, 0);

	mat4 view, proj, projview; //projview combine the view and proj
	
	//Camera Properties parameters
	float speed = 0.1f;
	float sensitivity = 100.0f;

	// Stores the width and height of the window
	int width, height;

	//Add speed and sensitivity to the camera movement and rotation (future)

	// Camera constructor to set up initial values
	Camera(int width, int height, vec3 position);

	~Camera()
	{
		std::cout << "invoked Camera" << std::endl;

	}

	// Orbit in the world space
	void Orbit(MeshResource* mesh); //ShaderResource& shaderProgram
	
	//void Lookat(float Fovdeg, float nearPlane, float farPlane, std::shared_ptr<ShaderResource> shader); // add the uniform params once we combine all camera uniform to one

	// Future add input handling to the camera (move according to the input)
	//void Inputs(int32_t key);
};
#endif