-- debug 模式需要做一些额外的设置
if is_mode("debug") then
    set_symbols("debug")
    set_optimize("none")
    add_defines("DEBUG")
    set_strip("none")
    add_cxflags("-g")
    add_ldflags("-g")
end

-- 编译 camera
target("camera")
    -- 设置目标的类型为静态库
    set_kind("static")

    -- 添加源文件
    add_files("./include/camera.cpp")

    -- 指定头文件的目录
    add_includedirs("./include")


target("stb_images")
    set_kind("static")
    add_files("./thirdParty/stbi_images.cpp")

target("mesh")
    set_kind("static")
    add_files("./include/shader.cpp")
    add_files("./include/mesh.cpp")
    add_files("./include/model.cpp")

    add_includedirs("./include")
    add_includedirs("./thirdParty")

target("assimp_model")
    set_kind("binary")

    add_files("./src/main.cpp")

    add_includedirs("./include/")
    add_includedirs("./thirdParty/")

    add_packages("assimp")

    add_links("glfw", "glad", "pthread", "assimp", "GL")
    add_deps("camera", "mesh", "stb_images")
