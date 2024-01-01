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

    private:
        // unsigned int framebuffer;
        // unsigned int textureColorbuffer;

    protected:
        GLFWWindowWrapper *window;
    };
} // namespace ht
