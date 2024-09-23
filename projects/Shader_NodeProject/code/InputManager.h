#ifndef INPUTMANAGER_CLASS_H
#define INPUTMANAGER_CLASS_H

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "GraphicsNode.h"
#include <config.h>

//HAVE TO FIGURE OUT HOW THE INPUT MANAGER WORKS

class InputManager
{
    float speed = 0.2f;
    vec3 newpos;
    double mouseX, mouseY; //testing
    public: 
        void inputsFromkeyboard(int32 key, std::shared_ptr<GraphicsNode> obj);
        void inputsFromMouse(int32 key,std::shared_ptr<GraphicsNode> obj, GLFWwindow* window, int winWeight, int winHeight);
};
#endif // !INPUTMANAGER_CLASS_H
#pragma region input reference
/*
 namespace Input2
{
    enum Keycode
    {
        SPACE,
        UP,
        DOWN,
        LEFT,
        RIGHT,
        W,
        NUM_KEYS
    };

    inline Keycode GetKeyCodeFromGLFW(int32 glfwKey)
    {
        switch (glfwKey)
        {
        case GLFW_KEY_SPACE: return Keycode::SPACE;
        case GLFW_KEY_UP: return Keycode::UP;
        case GLFW_KEY_DOWN: return Keycode::DOWN;
        case GLFW_KEY_LEFT: return Keycode::LEFT;
        case GLFW_KEY_RIGHT: return Keycode::RIGHT;
        case GLFW_KEY_W: return Keycode::W;
        default:
            printf("INVALID KEYCODE\n");
            assert(false);
            break;
        }
        return Keycode::SPACE;
    }

    struct Keyboard
    {
        bool pressed[Keycode::NUM_KEYS];
        bool held[Keycode::NUM_KEYS];
        bool released[Keycode::NUM_KEYS];
    };

    class InputManager
    {
    public:
        static InputManager* Instance()
        {
            static InputManager instance;
            return &instance;
        }

        Keyboard keyboard;

        void BeginFrame()
        {
            for (int i = 0; i < Keycode::NUM_KEYS; i++)
            {
                keyboard.pressed[i] = false;
                keyboard.released[i] = false;
            }
        }

        void HandleKeyEvent(int32 key, int32 action)
        {
            if (action == GLFW_PRESS)
            {
                keyboard.pressed[GetKeyCodeFromGLFW(key)] = true;
                keyboard.held[GetKeyCodeFromGLFW(key)] = true;
            }
            else if (action == GLFW_RELEASE)
            {
                keyboard.released[GetKeyCodeFromGLFW(key)] = true;
                keyboard.held[GetKeyCodeFromGLFW(key)] = false;
            }
        }

    private:
        static InputManager* instance;
        InputManager() {}
        ~InputManager() {}
    };
*/

#pragma endregion