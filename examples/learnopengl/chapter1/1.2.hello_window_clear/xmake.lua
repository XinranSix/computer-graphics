target('1.2.hello_window_clear')
    add_files("./**.cpp")
    -- set_rundir(path.directory(filepath))
    add_packages("opengl", "glfw", "glad")
