#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "window/glfwwindowwrapper.h"

namespace ht {
    class ImGuiRenderer {
    public:
        ImGuiRenderer(GLFWWindowWrapper *window);
        virtual void init();
        virtual ~ImGuiRenderer();
        virtual void Render();

    protected:
        GLFWWindowWrapper *window;
    };
} // namespace ht
