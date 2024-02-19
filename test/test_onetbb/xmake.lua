for _, filepath in ipairs(os.files("./**.cpp")) do
target('test_onetbb_' .. path.basename(filepath))
    add_files(filepath)
    add_packages("onetbb", "benchmark")
    add_deps("ht")
end
