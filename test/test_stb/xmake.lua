for _, filepath in ipairs(os.files("./**.cpp")) do
target('test_stb_' .. path.basename(filepath))
    add_files(filepath)
    add_packages("opengl", "glfw", "glad", "glm", "assimp", "stb")
    add_deps("imgui", "ht")
    after_build(function (target)
        os.cp("$(projectdir)/asset", target:targetdir())
    end)
end
