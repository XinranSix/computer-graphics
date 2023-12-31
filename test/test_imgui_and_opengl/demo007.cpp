#include "window/glfwwindowwrapper.h"

int main(int argc, char *argv[]) {
    ht::GLFWWindowWrapper window { 800, 600, "OPengl" };
    window.mainLoop();
    return 0;
}
