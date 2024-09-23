#pragma once

#include "core/math/mat4.h"
#include "core/math/vec3.h"


namespace Object
{
	class Camera
	{
	private:
		mat4 projection;
		mat4 projview;

		vec2 inputDir;
		vec3 position;
		vec3 direction;
		vec3 upVec;
		vec3 rightVec;

	public:
		mat4 view;

		Camera();
		~Camera() {};

		//Perspective
		void SetPerspective(float FoV, float win_width, float win_height);
		//Orthographic
		void SetOrthographic(float left, float right, float top, float bottom, float near, float far);

		void Freefly(vec3 input, float moveSpeed, float sensitivity, float delta_mouseX, float delta_mouseY, float deltaTime);
		void Orbit(float speed, float radius, float height, float deltaTime);

		inline void Setpos(const vec3 pos) { position = pos; }
		inline void SetView(const mat4& v) { view = v; }
		inline void SetCameraUp() { upVec = vec3(0, 1, 0); }

		inline vec3 GetPosition() const  { return position; }
		inline mat4 GetProjView() const { return projview; }
		inline vec3 GetDirection() const { return direction; }
		inline vec3 GetUP() const { return upVec; }
	};
}