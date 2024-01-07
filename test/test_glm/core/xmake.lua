for _, filepath in ipairs(os.files("./**.cpp")) do
target('test_gml_core_' .. path.basename(filepath))
    add_files(filepath)
    add_packages("glm")
    after_build(function (target)
        os.cp("$(projectdir)/asset", target:targetdir())
    end)
end
