add_includedirs("./include")
if is_plat("windows") then
    add_cxflags("/utf-8")
end

target('sb7')
    add_files("./sb7/**.cpp", "./sb7/**.c")
    set_kind("static")
    add_defines("WIN32", "_WINDOWS")
    add_packages("glfw")

for _, filepath in ipairs(os.files("./src/**.cpp")) do
target('sb7_' .. path.basename(filepath))
    add_files(filepath)
    add_packages("glfw", "glm")
    add_deps("sb7")
    add_defines("WIN32", "_WINDOWS")
    after_build(function (target)
        os.cp("$(projectdir)/asset", target:targetdir())
    end)
end
