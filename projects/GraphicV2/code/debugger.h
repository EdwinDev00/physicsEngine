//#pragma once
//
//#include <gl/glew.h>
//
//#include "vertexArray.h"
//#include "indexBuffer.h"
//#include "shaderResource.h"
//
////MACRO FOR ASSERT IF ENCOUNTER ERROR (BREAK INSTANTLY)
////#define ASSERT(x) if(!(x)) __debugbreak(); //__ Specific for msvc
////MACRO TO Execute the error clearing and checking (CALL THIS MACRO ON EVERY ON OPENGL FUNCTIONS)
////#define GLCall(x) GLClearError(); x;// ASSERT(GLLogCall(#x,__FILE__,__LINE__)); //x = the function we called, #x turn it into a string
//
//void GLClearError();
//
//bool GLLogCall(const char* function, const char* file, int line);
//
//