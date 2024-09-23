#pragma once

#include "ShaderResource.h"
#include "Camera.h"
#include <queue>

namespace Debug
{

	void Init();

	void DrawLine(glm::vec3 start, glm::vec3 end, glm::vec4 color, float lineWidth = 1);

	void DrawBox(glm::vec3 pos, glm::vec3 rot, glm::vec3 extents, glm::vec4 color, float lineWidth = 1);
	void DrawBox(glm::mat4 transform, glm::vec4 color, float lineWidth = 1);

	void DrawSphere(glm::vec3 pos, float radius, glm::vec4 color, float lineWidth = 1);

	void DrawPlane(glm::vec3 pos, glm::vec3 rot, glm::vec2 extents, glm::vec4 color, float lineWidth = 1);

	void ClearQueue();

	void RenderDebug(const Object::Camera& cam);
}