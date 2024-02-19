for _, filepath in ipairs(os.files("./**.cpp")) do
target('test_sdl_' .. path.basename(filepath))
    add_files(filepath)
    add_packages("libsdl")
    after_build(function (target)
        os.cp("$(projectdir)/asset", target:targetdir())
    end)
end
