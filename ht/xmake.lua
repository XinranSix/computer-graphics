target("ht")
    set_kind("static")
    add_files("./src/**.cpp")   
    add_packages("glad", "glm", "assimp", "stb")
