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