#ifndef __RENDERER_OPENGL__
#define __RENDERER_OPENGL__

#include <GL/glew.h>

#ifdef __clang__
// clang 和 clang++
    #define ASSERT(x) if(!(x)) __builtin_debugtrap();
#endif /* __clang__ */

#ifdef __GCC__
// gcc 和 g++
    #define ASSERT(x) if(!(x)) __builtin_trap();
#endif /* __GCC__ */

#ifdef __MSVC__
// windows MSVC cl 编译器
    #define ASSERT(x) if (!(x)) __debugbreak();
#endif

#define GLCALL(x) GLClearError(); \
    x; \
    ASSERT(GLCheckError(#x, __FILE__, __LINE__))


void GLClearError() ;

bool GLCheckError(const char* function, const char* file, int line);


#endif