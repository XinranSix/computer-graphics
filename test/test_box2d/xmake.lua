for _, filepath in ipairs(os.files("./**.cpp")) do
target('test_box2d_' .. path.basename(filepath))
    add_files(filepath)
    set_rundir(path.directory(filepath))
    add_packages("box2d")
end
