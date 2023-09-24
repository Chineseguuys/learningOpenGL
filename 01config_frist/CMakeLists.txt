cmake_minimum_required(VERSION 3.16.3)

project(config_frist)


set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_C_COMPILER "clang")
set (CMAKE_CXX_COMPILER "clang++")
set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_BUILD_TYPE "Debug")
#set (CMAKE_BUILD_TYPE "Release")


add_executable(${PROJECT_NAME}
    main.cpp    
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
)