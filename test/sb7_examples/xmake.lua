add_includedirs("./include")
add_cxflags("/utf-8")

target('sb7lib')
    add_files("./sb7/**.cpp", "./sb7/**.c")
    set_kind("static")
    add_defines("WIN32", "_WINDOWS")
    add_packages("glfw")

for _, filepath in ipairs(os.files("./src/**.cpp")) do
target("sb7_examples_" .. string.sub(filepath, 1, string.find(filepath, "\\") - 1) .. "_" .. path.basename(filepath))
    add_files(filepath)
    add_packages("glfw", "glm")
    add_deps("sb7lib")
    add_defines("WIN32", "_WINDOWS")
    set_rundir("$(projectdir)/test/sb7_examples")
end
