for _, filepath in ipairs(os.files("./**.cc")) do
target("test_fast_io_" .. string.sub(filepath, 1, string.find(filepath, "\\") - 1) .. "_" .. path.basename(filepath))
    add_files(filepath)
    set_rundir(path.directory(filepath))
    add_packages("fast_io")
end
