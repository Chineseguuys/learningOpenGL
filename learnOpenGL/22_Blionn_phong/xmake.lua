-- 查找 spdlog 库， 类似于 cmake 中的 find_package() 函数
if is_os("windows") then
    add_requires("vcpkg::spdlog", {alias = "spdlog"})
    add_requires("vcpkg::fmt", {alias = "fmt"})
    add_requires("vcpkg::glfw3", {alias = "glfw3"})
    add_requires("vcpkg::glm", {alias = "glm"})
elseif is_os("linux") then
    add_requires("spdlog", {system = true})
end

if is_os("windows") then
    set_toolchains("msvc")
    add_defines("UNICODE", "_UNICODE")  -- 启用 Unicode 支持
    add_cxflags("/utf-8")               -- 强制使用 UTF-8 编码
elseif is_os("linux") then
    set_toolchains("gcc")
end

add_defines("ENABLE_POINT_SIZE")

target("glad")
    set_kind("static")
    add_files("thirdParty/glad/src/glad.c")
    add_includedirs("thirdParty/glad/include/")

target("stb_image")
    set_kind("static")
    add_files("thirdParty/stbi_images.cpp")

target("camera")
    set_kind("static")
    add_files("include/camera.cpp")
    add_files("include/processInput.cpp")
    add_includedirs("include/")
    add_includedirs("thirdParty/glad/include/")
    if is_os("windows") then
        add_packages("glm")
    end

target("mesh")
    set_kind("static")
    add_files("include/shader.cpp")
    add_files("include/mesh.cpp")
    add_files("include/model.cpp")
    add_includedirs("include/")
    add_includedirs("thirdParty/")
    add_includedirs("thirdParty/glad/include/")
    if is_os("windows") then
        add_packages("glm")
        add_packages("assimp")
    end

target("imgui_impl_opengl3")
    set_kind("static")
    add_files("thirdParty/imgui/backends/imgui_impl_opengl3.cpp")
    add_files("thirdParty/imgui/imgui_demo.cpp")
    add_files("thirdParty/imgui/imgui_draw.cpp")
    add_files("thirdParty/imgui/imgui_tables.cpp")
    add_files("thirdParty/imgui/imgui_widgets.cpp")
    add_files("thirdParty/imgui/imgui.cpp")
    add_files("thirdParty/imgui/imgui_impl_glfw.cpp")
    add_includedirs("thirdParty/imgui")
    add_includedirs("thirdParty/glad/include/")
    if is_os("windows") then
        add_packages("glm")
    end

target("advanced_lighting")
    set_kind("binary")
    add_files("src/main.cpp")
    add_includedirs("include/")
    add_includedirs("thirdParty/")
    add_includedirs("thirdParty/imgui")
    add_includedirs("thirdParty/imgui/backends")
    add_includedirs("thirdParty/glad/include/")
    add_deps("stb_image", "mesh", "camera", "imgui_impl_opengl3", "glad")
    if is_os("windows") then
        add_packages("glfw3")
        add_packages("fmt")
        add_links("gdi32", "shell32", "user32", "glad", "opengl32", "stb_image", "mesh", "camera", "imgui_impl_opengl3")
    elseif is_os("linux") then
        add_links("glfw", "glad", "GL", "X11", "pthread", "Xrandr", "assimp", "stb_image", "mesh", "camera", "imgui_impl_opengl3")
    end
    -- 设置工作目录（支持相对路径和绝对路径
    -- 在使用 xmake project -k vsxmake -m "debug" 去为 xmake 项目生成 visual studio 项目的时候，很有作用。
    set_rundir("$(projectdir)/build")  -- 项目根目录下的 bin 文件夹
