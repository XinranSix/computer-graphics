for _, filepath in ipairs(os.files("./**.cpp")) do
target('test_spdlog_' .. path.basename(filepath))
    add_files(filepath)
    add_packages("spdlog")
    add_deps("ht")
    after_build(function (target)
        os.cp("$(projectdir)/asset", target:targetdir())
    end)
end
