#include "InputManager.h"

void InputManager::inputsFromkeyboard(int32 key, std::shared_ptr<GraphicsNode> obj)
{
	//Handle Object Inputs

	// MOVEMENT
	if (key == GLFW_KEY_UP)
	{
		//bug when moved will reset all the rotation applied
		newpos = vec3(obj->transform.m[3].x, obj->transform.m[3].y, obj->transform.m[3].z + -speed);
		obj->transform = translate(newpos);
	}

	if (key == GLFW_KEY_LEFT)
	{
		newpos = vec3(obj->transform.m[3].x + -speed, obj->transform.m[3].y, obj->transform.m[3].z);
		obj->transform = translate(newpos);
	}

	if (key == GLFW_KEY_DOWN)
	{
		newpos = vec3(obj->transform.m[3].x, obj->transform.m[3].y, obj->transform.m[3].z + speed);
		obj->transform = translate(newpos);
	}
	if (key == GLFW_KEY_RIGHT)
	{
		newpos = vec3(obj->transform.m[3].x + speed, obj->transform.m[3].y, obj->transform.m[3].z);
		obj->transform = translate(newpos);
	}
}

void InputManager::inputsFromMouse(int32 key, std::shared_ptr<GraphicsNode> obj, GLFWwindow* window, int winWeight, int winHeight)
{
	//mouse input (Rotation) scuffed handle
	if (key == GLFW_MOUSE_BUTTON_LEFT)
	{
		//need some refinement on this
		glfwGetCursorPos(window, &mouseX, &mouseY);
		/*	std::cerr << "MouseX value: " << mouseX << "\n";
			std::cerr << "MouseY value: " << mouseY << "\n";*/
		if ((winHeight / 2) < mouseY && mouseX < winWeight)
		{
			//std::cerr << "control Y value: " << ((s_height / 2) + (s_height / 4)) << "\n";
			obj->rotationX();
		}
		else if (winHeight > mouseY && mouseX < winWeight)
		{
			obj->rotationY();
		}
	}
}
