#include "window/GLFWWindowWithImGuiWrapper.h"
#include "imguirenderer/rendertoimguiconfigrenderer.h"
#include "factory/make_window.hpp"

class MyWindow : public ht::GLFWWindowWithImGuiWrapper {
    using GLFWWindowWithImGuiWrapper::GLFWWindowWithImGuiWrapper;
};

int main(int argc, char *argv[]) {
    auto window { ht::make_window<MyWindow, ht::RenderToImGuiConfigRenderer>(
        800, 600, "OpenGL") };
    window->run();
    return 0;
}
