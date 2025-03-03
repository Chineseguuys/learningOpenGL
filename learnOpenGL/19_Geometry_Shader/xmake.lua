-- 查找 spdlog 库， 类似于 cmake 中的 find_package() 函数
add_requires("spdlog", {system = true})

set_toolchains("gcc")

add_defines("ENABLE_POINT_SIZE")

target("stb_image")
    set_kind("static")
    add_files("thirdParty/stbi_images.cpp")

target("camera")
    set_kind("static")
    add_files("include/camera.cpp")
    add_files("include/processInput.cpp")
    add_includedirs("include/")

target("mesh")
    set_kind("static")
    add_files("include/shader.cpp")
    add_files("include/mesh.cpp")
    add_files("include/model.cpp")
    add_includedirs("include/")
    add_includedirs("thirdParty/")

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

target("geometry_shader")
    set_kind("binary")
    add_files("src/main.cpp")
    add_includedirs("include/")
    add_includedirs("thirdParty/")
    add_includedirs("thirdParty/imgui")
    add_includedirs("thirdParty/imgui/backends")

    -- add RenderDoc lib search dir
    -- add_rpathdirs("/home/yanjiangha/.local_software/renderdoc/renderdoc_1.35/lib/")

    add_deps("stb_image", "mesh", "camera", "imgui_impl_opengl3")
    add_links("glfw", "glad", "GL", "X11", "pthread", "Xrandr", "assimp", "stb_image", "mesh", "camera", "imgui_impl_opengl3")
