set_project("computer-graphics")
set_version("1.0.0")

add_rules("mode.debug", "mode.release")

set_languages("c++23")

pathSeparator = "/"

if is_plat("windows") then
    -- 由于 msvc 默认不认识 utf-8 编码
    -- 使用 msvc 是使用添加 /utf-8 标识符
    add_cxflags("/utf-8")
    pathSeparator = "\\"
end

add_includedirs("include")

-- 子模块
includes("src")
includes("test")
includes("examples")

-- 包配置文件
includes("package.lua")
