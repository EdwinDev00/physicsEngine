#pragma once

#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include"../glm/gtx/transform.hpp"
#include "../glm/gtc/quaternion.hpp"

inline glm::mat4 SetRotation(glm::vec3 const& v)
{
	return
		glm::rotate(v.x, glm::vec3(1, 0, 0)) *
		glm::rotate(v.y, glm::vec3(0, 1, 0)) *
		glm::rotate(v.z, glm::vec3(0, 0, 1));
}

inline float Distance(const glm::vec3& b, const glm::vec3& end) 
{
	return sqrtf
		(
		 powf((b.x - end.x), 2) +
			powf((b.y - end.y), 2) +
			powf((b.z - end.z), 2)
		);
}