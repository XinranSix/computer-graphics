#include "window/glfwwindowwithimguiwrapper.h"
#include "imguirenderer/imguirenderer.h"
#include "log/log.h"

int main(int argc, char *argv[]) {
    ht::Log::init();
    ht::GLFWWindowWithImGuiWrapper window { 800, 600, "OpenGL" };
    window.setImGuiRender(new ht::ImGuiRenderer { &window });
    window.init();
    window.run();
    return 0;
}
