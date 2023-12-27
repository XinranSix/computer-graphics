for _, filepath in ipairs(os.files("./**.cpp")) do
target('test_libigl_' .. path.basename(filepath))
    add_files(filepath)
    set_rundir(path.directory(filepath))
    add_packages("libigl")
end
