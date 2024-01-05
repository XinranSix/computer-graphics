for _, filepath in ipairs(os.files("./**.cpp")) do
target('test_tbb_' .. path.basename(filepath))
    add_files(filepath)
    add_packages("cxxopts", "opengl", "glad", "glfw", "onetbb", "openmp")
    add_deps("ht")
    after_build(function (target)
        os.cp("$(projectdir)/asset", target:targetdir())
    end)
end
