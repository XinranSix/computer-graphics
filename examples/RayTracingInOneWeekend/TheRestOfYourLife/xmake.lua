target('the_rest_of_your_life')
    add_files("./main.cpp")
    add_packages("stb")
    set_rundir("$(scriptdir)")
    after_build(function (target)
        os.cp("$(projectdir)/asset", target:targetdir())
    end)

target('estimate_of_pi')
    add_files("./pi.cpp")

target('integrate_x_sq')
    add_files("./integrate_x_sq.cpp")

target('estimate_halfway')
    add_files("./estimate_halfway.cpp")

target('sphere_importance')
    add_files("./sphere_importance.cpp")
    
target('cos_density')
    add_files("./cos_density.cpp")

target('cos_cubed')
    add_files("./cos_cubed.cpp")
    




