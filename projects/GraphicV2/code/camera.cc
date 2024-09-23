#include "camera.h"

namespace Object
{
	Camera::Camera()
		: position(vec3()),
		view(mat4()),
		projection(mat4()),
		direction(vec3()),
		upVec(vec3(0,1,0)),
		rightVec(vec3(1,0,0))
	{
	}

	void Camera::SetPerspective(float FoV, float win_width, float win_height)
	{
		projection = perspective(FoV, win_width/win_height,0.1f, 10000);
		this->projview = projection * view;
	}

	void Camera::SetOrthographic(float left, float right, float top, float bottom, float near, float far)
	{
		projection = orthographic(left,right,top,bottom,near,far);
		this->projview = projection * view;
	}
	void Camera::Freefly(vec3 input,float moveSpeed,float sensitivity, float delta_mouseX, float delta_mouseY, float deltaTime)
	{

		this->inputDir.x -= delta_mouseX * deltaTime * sensitivity;
		this->inputDir.y -= delta_mouseY * deltaTime * sensitivity;

		// Lock y direction so it doesnt loop around
		if (this->inputDir.y > (PI / 2) - 0.1f)
			this->inputDir.y = (PI / 2) - 0.1f;
		if (this->inputDir.y < -(PI / 2) + 0.1f)
			this->inputDir.y = -(PI / 2) + 0.1f;


		this->direction = normalize(vec3(-cos(inputDir.x) * cos(inputDir.y),
			inputDir.y, sin(inputDir.x) * cos(inputDir.y)));

		//move
		this->position +=
			//forward
			this->direction * input.z * deltaTime * moveSpeed
			//right
			+ normalize(cross(this->direction, this->upVec)) * input.x * deltaTime * moveSpeed
			//up
			+ this->upVec * input.y * deltaTime * moveSpeed;

		this->view = lookat(this->position, this->position + this->direction, -this->upVec);

		this->projview = projection * view;
	}


	void Camera::Orbit(float speed, float radius, float height, float deltaTime) 
	{
		this->position.x = sin(deltaTime * speed) * radius;
		this->position.y = height;
		this->position.z = cos(deltaTime * speed) * radius;


		this->view = lookat(this->position, vec3(0, 0, 0), this->upVec);
		this->projview = projection * view;
	}

}