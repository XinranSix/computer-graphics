#include "window/glfwwindowwithimguiwrapper.h"
#include "imguirenderer/imguiconfigrenderer.h"
#include "log/log.h"

int main(int argc, char *argv[]) {
    ht::Log::init();
    ht::GLFWWindowWithImGuiWrapper window { 800, 600, "OPengl" };
    window.setImGuiRender(new ht::ImGuiConfigRenderer { &window });
    window.init();
    window.run();
    return 0;
}
