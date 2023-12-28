for _, filepath in ipairs(os.files("./**.cpp")) do
target('test_others_' .. path.basename(filepath))
    add_files(filepath)
    set_rundir(path.directory(filepath))
    add_packages("gtest", "fmt", "tinyxml2", "yaml-cpp", "rapidjson", "jsoncpp", "protobuf-cpp", "easyloggingpp", "gflags")
end
