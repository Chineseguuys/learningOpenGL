-- 查找 spdlog 库， 类似于 cmake 中的 find_package() 函数
add_requires("spdlog", {system = true})

set_toolchains("gcc")

add_defines("ENABLE_DEPTH_TEST")
add_defines("ENABLE_MULTI_SAMPLE")

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
    add_deps("stb_image")
    add_links("stb_image")

target("offscreen_msaa")
    set_kind("binary")
    add_files("src/main.cpp")
    add_includedirs("include/")
    add_includedirs("thirdParty/")

    add_deps("stb_image", "mesh", "camera")
    add_links("glfw", "glad", "GL", "X11", "pthread", "Xrandr", "assimp", "stb_image", "mesh", "camera")
