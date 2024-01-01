#include "window/glfwwindowwithimguiwrapper.h"
#include "imguirenderer/rendertoimguiconfigrenderer.h"
#include "log/log.h"

int main(int argc, char *argv[]) {
    ht::Log::init();
    ht::GLFWWindowWithImGuiWrapper window { 800, 600, "OPengl" };
    window.setImGuiRender(new ht::RenderToImGuiConfigRenderer { &window });
    window.init();
    window.run();
    return 0;
}
