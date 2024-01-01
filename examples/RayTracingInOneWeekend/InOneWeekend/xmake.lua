target('InOneWeekend')
    add_files("./**.cpp")
    add_packages("glm", "stb")
    add_deps("ht")
    set_rundir("$(curdir)")
    after_build(function (target)
        os.cp("$(projectdir)/asset", target:targetdir())
    end)




