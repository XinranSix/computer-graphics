for _, filepath in ipairs(os.files("./**.cpp")) do
target('test_sdl_' .. path.basename(filepath))
    add_files(filepath)
    add_packages("libsdl", "opengl", "glm", "glad")
    set_rundir("$(projectdir)")
end
