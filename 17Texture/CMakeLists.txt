cmake_minimum_required(VERSION 3.16.3)

project(texture)

# some variables 如果使用 llvm 的链接器，需要加上下面的 flags
set (LINK_FLAGS "-lc++ -lc++abi -fuse-ld=lld")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_C_COMPILER "clang")
set (CMAKE_CXX_COMPILER "clang++")
set (CMAKE_VERBOSE_MAKEFILE ON)

# debug mode
#set (CMAKE_BUILD_TYPE "Debug")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")


set (CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DRELEASE")

# 要完全使用 clang 编译器，就需要链接 libc++ 以及 libc++abi。以及使用链接器 lld 如果使用 gcc就不需要这些选项
set (CMAKE_SHARED_LINKER_FLAGS "${LINK_FLAGS}")
set (CMAKE_EXE_LINKER_FLAGS "${LINK_FLAGS}")

# 用于 OpenGL 中的程序的错误打印
add_library(error_check STATIC
    ${PROJECT_SOURCE_DIR}/src/ErrorCheck.cpp
)

# 顶点缓冲，索引缓冲，顶点数组和 顶点数组 layout
add_library(buffers STATIC
    ${PROJECT_SOURCE_DIR}/src/VertexBuffer.cpp
    ${PROJECT_SOURCE_DIR}/src/IndexBuffer.cpp
    ${PROJECT_SOURCE_DIR}/src/VertexArray.cpp
    ${PROJECT_SOURCE_DIR}/src/VertexBufferLayout.cpp
)

# 顶点着色器和片段着色器的包装类
add_library(shader STATIC
    ${PROJECT_SOURCE_DIR}/src/Shader.cpp
)

# 一个简单的渲染器
add_library(renderer STATIC 
    ${PROJECT_SOURCE_DIR}/src/Renderer.cpp
)

# 一个简单的纹理类
add_library(textures STATIC
    ${PROJECT_SOURCE_DIR}/src/Texture.cpp
)

# stb images header only 这是一个图像解析库 需要自己建立一个独立的 cpp 文件来引入它
add_library(stb_image STATIC
    ${PROJECT_SOURCE_DIR}/src/vendor/stb_image/stb_image.cpp
)

# 入口程序
add_executable(${PROJECT_NAME}
    main.cpp
)

# 头文件位置
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/src/vendor/stb_image
)

# 编译 OpenGL 程序需要链接的动态库 + 代码中生成的静态库
target_link_libraries(${PROJECT_NAME}
    GLEW 
    glfw3
    glad
    GL 
    X11
    pthread
    Xrandr 
    Xi
    ${CMAKE_DL_LIBS} # Name of library containing ``dlopen`` and ``dlclose``.
    error_check
    buffers
    shader
    renderer
    textures
    stb_image
)