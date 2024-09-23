#pragma once

#include "ShaderResource.h"
#include "Camera.h"
#include <queue>

namespace Debug
{

	void Init();

	void DrawLine(vec3 start, vec3 end, vec4 color, float lineWidth = 1);

	void DrawBox(vec3 pos, vec3 rot, vec3 extents, vec4 color, float lineWidth = 1);
	void DrawBox(mat4 transform, vec4 color, float lineWidth = 1);

	void DrawSphere(vec3 pos, float radius, vec4 color, float lineWidth = 1);

	void DrawPlane(vec3 pos, vec3 rot, vec2 extents, vec4 color, float lineWidth = 1);

	void ClearQueue();

	void RenderDebug(const Object::Camera& cam);
}