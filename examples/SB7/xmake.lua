add_includedirs("./include")

for _, filepath in ipairs(os.files("./src/**.cpp")) do
target('sb7_' .. path.basename(filepath))
    add_files(filepath)
    add_packages("opengl", "glfw", "glew")
    after_build(function (target)
        os.cp("$(projectdir)/asset", target:targetdir())
    end)
end
