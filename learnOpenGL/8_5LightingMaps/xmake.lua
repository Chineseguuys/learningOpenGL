set_toolchains("gcc")

-- debug 模式需要做一些额外的设置
if is_mode("debug") then
    set_symbols("debug")
    set_optimize("none")
    add_defines("DEBUG")
    set_strip("none")
    add_cxflags("-g")
    add_ldflags("-g")
end

-- 编译 shader
target("shader")
    set_kind("static")
    add_files("./source/Shader.cpp")

-- 编译 camera
target("camera")
    -- 设置目标的类型为静态库
    set_kind("static")

    -- 添加源文件
    add_files("./source/Camera.cpp")

-- 编译 stb_images
target("stb_images")
    set_kind("static")
    add_files("./source/stbi_images.cpp")

target("imgui_impl_opengl3")
    set_kind("static")
    add_files("./source/imgui/backends/imgui_impl_opengl3.cpp")
    add_files("./source/imgui/imgui_demo.cpp")
    add_files("./source/imgui/imgui_draw.cpp")
    add_files("./source/imgui/imgui_tables.cpp")
    add_files("./source/imgui/imgui_widgets.cpp")
    add_files("./source/imgui/imgui.cpp")
    add_files("source/imgui/imgui_impl_glfw.cpp")

    add_includedirs("./source/imgui/")

target("lighting_maps")
    set_kind("binary")

    add_files("main.cpp")

    add_includedirs("./source/")
    add_includedirs("./source/imgui/")
    add_includedirs("./source/imgui/backends")

    add_links("glfw", "glad", "pthread", "GL")
    add_deps("camera", "stb_images", "imgui_impl_opengl3", "shader")
