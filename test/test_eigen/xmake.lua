for _, filepath in ipairs(os.files("./**.cpp")) do
target('test_test_eigen_' .. path.basename(filepath))
    add_files(filepath)
    add_packages("eigen")
    after_build(function (target)
        os.cp("$(projectdir)/asset", target:targetdir())
    end)
end
