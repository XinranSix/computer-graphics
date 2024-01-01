#include "window/glfwwindowwrapper.h"
#include "log/log.h"

int main(int argc, char *argv[]) {
    ht::Log::init();
    ht::GLFWWindowWrapper window { 800, 600, "OpenGL" };
    window.init();
    window.run();
    return 0;
}
