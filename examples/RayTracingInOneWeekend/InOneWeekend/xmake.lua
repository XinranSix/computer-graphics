target('in_one_weekend')
    add_files("./**.cpp")
    set_rundir("$(curdir)")
    after_build(function (target)
        os.cp("$(projectdir)/asset", target:targetdir())
    end)




