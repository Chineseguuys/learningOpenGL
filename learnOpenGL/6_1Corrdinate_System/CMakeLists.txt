cmake_minimum_required(VERSION 3.16.3)

project(corrdinate_system)

# some variables 如果使用 llvm 的链接器，需要加上下面的 flags
#set (LINK_FLAGS "-lc++ -lc++abi -fuse-ld=lld")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
#set (CMAKE_C_COMPILER "clang")
#set (CMAKE_CXX_COMPILER "clang++")
set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_BUILD_TYPE "Debug")
#set (CMAKE_BUILD_TYPE "Release")

# 要完全使用 clang 编译器，就需要链接 libc++ 以及 libc++abi。以及使用链接器 lld 如果使用 gcc就不需要这些选项
set (CMAKE_SHARED_LINKER_FLAGS "${LINK_FLAGS}")
set (CMAKE_EXE_LINKER_FLAGS "${LINK_FLAGS}")

add_library(shader STATIC
    ${PROJECT_SOURCE_DIR}/source/Shader.cpp
)

add_library(stb_image STATIC
    ${PROJECT_SOURCE_DIR}/source/stbi_images.cpp
)

# imgui 库的引入。该库可以方便我们实时进行相关参数的设置
# 下面的问题拷贝自 ImGui 的仓库的根目录以及 backends 目录。我们只关注 glfw 和 OpenGL3 部分
add_library(imgui_impl_opengl3
    ${PROJECT_SOURCE_DIR}/source/imgui/backends/imgui_impl_opengl3.cpp
    ${PROJECT_SOURCE_DIR}/source/imgui/imgui_demo.cpp
    ${PROJECT_SOURCE_DIR}/source/imgui/imgui_draw.cpp
    ${PROJECT_SOURCE_DIR}/source/imgui/imgui_tables.cpp
    ${PROJECT_SOURCE_DIR}/source/imgui/imgui_widgets.cpp
    ${PROJECT_SOURCE_DIR}/source/imgui/imgui.cpp
    ${PROJECT_SOURCE_DIR}/source/imgui/imgui_impl_glfw.cpp
)

# 注意目录之间的引用关系，backends 的代码位于子目录，当然也可以放在同一个目录当中，这里为了和源仓库中保持一直
# 没有放置在一起。因此添加下面的 cmake include 参数
target_include_directories(imgui_impl_opengl3
    PUBLIC
        ${PROJECT_SOURCE_DIR}/source/imgui
)

add_executable(${PROJECT_NAME}
    main.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/source
        ${PROJECT_SOURCE_DIR}/source/imgui
        ${PROJECT_SOURCE_DIR}/source/imgui/backends
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

    shader
    stb_image
    imgui_impl_opengl3
)
