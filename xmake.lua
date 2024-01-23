set_project("computer-graphics")
set_version("1.0.0")

add_rules("mode.debug", "mode.release", "mode.releasedbg", "mode.minsizerel", "mode.check", "mode.profile", "mode.coverage", "mode.valgrind", "mode.asan", "mode.tsan", "mode.lsan", "mode.ubsan")

set_languages("c++23")

pathSeparator = "/"

if is_plat("windows") then
    -- 由于 msvc 默认不认识 utf-8 编码
    -- 使用 msvc 是使用添加 /utf-8 标识符
    add_cxflags("/utf-8")
    pathSeparator = "\\"
end

add_includedirs("include")

-- add_defines("STB_IMAGE_IMPLEMENTATION")

-- 子模块
includes("src")
includes("test")
includes("examples")
includes("ht")

-- 包配置文件
includes("package.lua")

target("imgui")
    add_files("./imgui/**.cpp")
    set_kind("static")
    add_packages("glfw", "glad", "stb", "dirent")
