for _, filepath in ipairs(os.files("./**.cpp")) do
target('test_magic_enum_' .. path.basename(filepath))
    add_files(filepath)
    add_packages("rttr")
    after_build(function (target)
        os.cp("$(projectdir)/asset", target:targetdir())
    end)
end
