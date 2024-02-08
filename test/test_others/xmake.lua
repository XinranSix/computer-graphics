for _, filepath in ipairs(os.files("./**.cpp")) do
target('test_others_' .. path.basename(filepath))
    add_files(filepath)
    add_packages("cxxopts", "opengl", "glad", "glfw", "glm", "sfml", "gtest")
    add_deps("ht")
    after_build(function (target)
        os.cp("$(projectdir)/asset", target:targetdir())
    end)
end
