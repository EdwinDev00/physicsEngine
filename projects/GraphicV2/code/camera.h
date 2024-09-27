#pragma once

#include "config.h"
//#include "core/math/mat4.h"
//#include "core/math/vec3.h"

class Ray;

namespace Object
{
	class Camera
	{
	private:
		glm::mat4 projection;
		glm::mat4 projview;

		glm::vec2 inputDir;
		glm::vec3 position;
		glm::vec3 direction;
		glm::vec3 upVec;
		glm::vec3 rightVec;

	public:
		glm::mat4 view;

		Camera();
		~Camera() {};

		//Perspective
		void SetPerspective(float FoV, float win_width, float win_height);
		//Orthographic
		void SetOrthographic(float left, float right, float top, float bottom, float near, float far);

		void Freefly(glm::vec3 input, float moveSpeed, float sensitivity, float delta_mouseX, float delta_mouseY, float deltaTime);
		void Orbit(float speed, float radius, float height, float deltaTime);

		inline void Setpos(const glm::vec3 pos) { position = pos; }
		inline void SetView(const glm::mat4& v) { view = v; }
		inline void SetCameraUp() { upVec = glm::vec3(0, 1, 0); }

		inline glm::vec3 GetPosition() const  { return position; }
		inline glm::mat4 GetProjView() const { return projview; }
		inline glm::mat4 GetProjMat() const { return projection; }
		inline glm::mat4 GetViewMat() const { return view; }

		inline glm::vec3 GetDirection() const { return direction; }
		inline glm::vec3 GetUP() const { return upVec; }
	};
}