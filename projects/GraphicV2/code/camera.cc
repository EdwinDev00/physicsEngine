#include "camera.h"
#define PI 3.141592653589793238f

namespace Object
{
	Camera::Camera()
		: /*position(vec3()),
		view(mat4()),
		projection(mat4()),
		direction(vec3()),*/
		upVec(glm::vec3(0,1,0)),
		rightVec(glm::vec3(1,0,0))
	{
	}

	void Camera::SetPerspective(float FoV, float win_width, float win_height)
	{
		projection = glm::perspectiveFov(FoV, win_width, win_height, 0.1f, 10000.0f);
		//projection = glm::perspective(FoV, win_width / win_height, 0.1f, 10000.0f);
		this->projview = projection * view;
	}

	void Camera::SetOrthographic(float left, float right, float top, float bottom, float near, float far)
	{
		projection = glm::ortho(left,right,top,bottom,near,far);
		/*projection = glm::orthoLH(left, right, top, bottom, near, far);
		projection = glm::orthoRH(left, right, top, bottom, near, far);*/

		this->projview = projection * view;
	}
	void Camera::Freefly(glm::vec3 input,float moveSpeed,float sensitivity, float delta_mouseX, float delta_mouseY, float deltaTime)
	{

		this->inputDir.x -= delta_mouseX * deltaTime * sensitivity;
		this->inputDir.y -= delta_mouseY * deltaTime * sensitivity;

		// Lock y direction so it doesnt loop around
		if (this->inputDir.y > (PI / 2) - 0.1f)
			this->inputDir.y = (PI / 2) - 0.1f;
		if (this->inputDir.y < -(PI / 2) + 0.1f)
			this->inputDir.y = -(PI / 2) + 0.1f;


		this->direction = normalize(glm::vec3(-cos(inputDir.x) * cos(inputDir.y),
			inputDir.y, sin(inputDir.x) * cos(inputDir.y)));

		//move
		this->position +=
			//forward
			this->direction * input.z * deltaTime * moveSpeed
			//right
			+ normalize(cross(this->direction, this->upVec)) * input.x * deltaTime * moveSpeed
			//up
			+ this->upVec * input.y * deltaTime * moveSpeed;

		this->view = glm::lookAt(this->position, this->position + this->direction, -this->upVec);

		this->projview = projection * view;
	}


	void Camera::Orbit(float speed, float radius, float height, float deltaTime) 
	{
		this->position.x = sin(deltaTime * speed) * radius;
		this->position.y = height;
		this->position.z = cos(deltaTime * speed) * radius;


		this->view = glm::lookAt(this->position, glm::vec3(0, 0, 0), this->upVec);
		this->projview = projection * view;
	}

}