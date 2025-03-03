cmake_minimum_required(VERSION 3.16.3)

project(vertex_array)

# some variables 如果使用 llvm 的链接器，需要加上下面的 flags
set (LINK_FLAGS "-lc++ -lc++abi -fuse-ld=lld")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_C_COMPILER "clang")
set (CMAKE_CXX_COMPILER "clang++")
set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_BUILD_TYPE "Debug")
#set (CMAKE_BUILD_TYPE "Release")

# 要完全使用 clang 编译器，就需要链接 libc++ 以及 libc++abi。以及使用链接器 lld 如果使用 gcc就不需要这些选项
set (CMAKE_SHARED_LINKER_FLAGS "${LINK_FLAGS}")
set (CMAKE_EXE_LINKER_FLAGS "${LINK_FLAGS}")

add_library(renderer STATIC
    ${PROJECT_SOURCE_DIR}/src/Renderer.cpp
)

add_library(buffers STATIC
    ${PROJECT_SOURCE_DIR}/src/VertexBuffer.cpp
    ${PROJECT_SOURCE_DIR}/src/IndexBuffer.cpp
)

add_executable(${PROJECT_NAME}
    main.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)

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
    renderer
    buffers
)