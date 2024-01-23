if is_plat("windows") then
    add_cxflags("/utf-8")
end

add_includedirs("include")

for _, filepath in ipairs(os.files("./src/**.cpp")) do
target('shadertoy_' .. path.basename(filepath))
    add_files(filepath)
    add_packages("glad", "glm", "assimp", "stb", "spdlog", "glfw", "magic_enum")
    add_deps("imgui")
    after_build(function (target)
        os.cp("$(projectdir)/asset", target:targetdir())
    end)
end
