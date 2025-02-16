-- 查找 spdlog 库， 类似于 cmake 中的 find_package() 函数
if is_os("windows") then
    add_requires("vcpkg::spdlog", {alias = "spdlog"})
    add_requires("vcpkg::fmt", {alias = "fmt"})
    add_requires("vcpkg::glfw3", {alias = "glfw3"})
    add_requires("vcpkg::glm", {alias = "glm"})
elseif is_os("linux") then
    add_requires("spdlog", {system = true})
end

-- 设置调试模式
-- add_rules("mode.debug")
if is_plat("windows") then
    -- windows 下使用 msvc 进行编译
    set_toolchains("msvc")
    add_defines("UNICODE", "_UNICODE")  -- 启用 Unicode 支持
    add_cxflags("/utf-8")               -- 强制使用 UTF-8 编码
    set_languages("cxx17")  -- 设置 C++17 标准
    add_cxflags("/ZI", "/FS")  -- 启用编辑继续调试
    --set_pcxxheader("pch.h")    -- 可选：预编译头加速调试
elseif is_os("linux") then
    set_toolchains("gcc")
end

target("glad")
    set_kind("static")
    add_files("thirdParty/glad/v_4.3/src/glad.c")
    add_includedirs("thirdParty/glad/v_4.3/include/")

target("stb_image")
    set_kind("static")
    add_files("thirdParty/stbi_images.cpp")

target("camera")
    set_kind("static")
    add_files("include/camera.cpp")
    add_files("include/processInput.cpp")
    add_includedirs("include/")
    add_includedirs("thirdParty/glad/v_4.3/include/")
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
    add_includedirs("thirdParty/glad/v_4.3/include/")
    if is_os("windows") then
        add_packages("glm")
        add_packages("assimp")
    end

target("normal_mapping")
    set_kind("binary")
    add_files("src/main.cpp")
    add_includedirs("include/")
    add_includedirs("thirdParty/")
    add_includedirs("thirdParty/glad/v_4.3/include/")
    add_deps("stb_image", "mesh", "camera", "glad")
    if is_os("windows") then
        add_packages("glfw3")
        add_packages("fmt")
        add_links("gdi32", "shell32", "user32", "glad", "opengl32", "stb_image", "mesh", "camera")
    elseif is_os("linux") then
        add_links("glfw", "glad", "GL", "X11", "pthread", "Xrandr", "assimp", "stb_image", "mesh", "camera")
    end

    -- 设置工作目录（支持相对路径和绝对路径
    -- 在使用 xmake project -k vsxmake -m "debug" 去为 xmake 项目生成 visual studio 项目的时候，很有作用。
    -- 在 Lua 中配置了下面的选项之后，就不需要在 xmake run 后使用 --workdir=build 的参数选项了
    set_rundir("$(projectdir)/build")  -- 项目根目录下的 bin 文件夹
