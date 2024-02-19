includes("test_fast_io")
includes("test_others")
includes("test_magic_enum")
includes("test_libigl")
includes("test_catch2")
includes("test_box2d")
includes("test_imgui_and_opengl")
includes("test_eigen")
includes("test_sdl")
includes("test_boost")
includes("test_opencv")
includes("test_spdlog")
includes("test_onetbb")


if is_plat("windows") then
    includes("sb7_examples")
end
