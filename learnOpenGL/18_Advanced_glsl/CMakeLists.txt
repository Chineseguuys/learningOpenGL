cmake_minimum_required(VERSION 3.16.3)

project(advanced_glsl)

# some variables 如果使用 llvm 的链接器，需要加上下面的 flags
#set (LINK_FLAGS "-fuse-ld=lld")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_BUILD_TYPE "Debug")
#set (CMAKE_BUILD_TYPE "Release")

add_definitions(-DENABLE_POINT_SIZE)

# 要完全使用 clang 编译器，就需要链接 libc++ 以及 libc++abi。以及使用链接器 lld 如果使用 gcc就不需要这些选项
set (CMAKE_SHARED_LINKER_FLAGS "${LINK_FLAGS}")
set (CMAKE_EXE_LINKER_FLAGS "${LINK_FLAGS}")

find_package(spdlog REQUIRED)

add_library(stb_image STATIC
    ${PROJECT_SOURCE_DIR}/thirdParty/stbi_images.cpp
)

add_library(camera STATIC
        ${PROJECT_SOURCE_DIR}/include/camera.cpp)

add_library(mesh STATIC
    ${PROJECT_SOURCE_DIR}/include/shader.cpp
    ${PROJECT_SOURCE_DIR}/include/mesh.cpp
    ${PROJECT_SOURCE_DIR}/include/model.cpp
    )

add_library(imgui_impl_opengl3
    ${PROJECT_SOURCE_DIR}/thirdParty/imgui/backends/imgui_impl_opengl3.cpp
    ${PROJECT_SOURCE_DIR}/thirdParty/imgui/imgui_demo.cpp
    ${PROJECT_SOURCE_DIR}/thirdParty/imgui/imgui_draw.cpp
    ${PROJECT_SOURCE_DIR}/thirdParty/imgui/imgui_tables.cpp
    ${PROJECT_SOURCE_DIR}/thirdParty/imgui/imgui_widgets.cpp
    ${PROJECT_SOURCE_DIR}/thirdParty/imgui/imgui.cpp
    ${PROJECT_SOURCE_DIR}/thirdParty/imgui/imgui_impl_glfw.cpp
)

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/src/main.cpp)

target_include_directories(mesh
        PUBLIC
        ${PROJECT_SOURCE_DIR}/thirdParty)

target_include_directories(camera
        PUBLIC
        ${PROJECT_SOURCE_DIR}/thirdParty
        ${PROJECT_SOURCE_DIR}/include
)

target_include_directories(imgui_impl_opengl3
    PUBLIC
        ${PROJECT_SOURCE_DIR}/thirdParty/imgui)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/thirdParty
        ${PROJECT_SOURCE_DIR}/thirdParty/imgui
        ${PROJECT_SOURCE_DIR}/thirdParty/imgui/backends
        /user/local/include
)

# 未知的原因导致这个目录没有默认在编译器的搜索路径里面
target_link_directories(${PROJECT_NAME}
    PUBLIC
        /usr/local/lib/
)

target_link_libraries(${PROJECT_NAME}
    #GLEW
    glfw
    glad
    GL
    X11
    pthread
    Xrandr
    Xi
    ${CMAKE_DL_LIBS} # Name of library containing ``dlopen`` and ``dlclose``.

    assimp
    stb_image
    mesh
    camera
    imgui_impl_opengl3
    spdlog::spdlog
)
