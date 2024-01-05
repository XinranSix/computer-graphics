target('the_next_week')
    add_files("./**.cpp")
    add_packages("stb")
    set_rundir("$(scriptdir)")
    after_build(function (target)
        os.cp("$(projectdir)/asset", target:targetdir())
    end)




