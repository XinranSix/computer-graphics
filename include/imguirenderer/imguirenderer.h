#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace ht {
    class ImGuiRenderer {
    public:
        ImGuiRenderer(GLFWwindow *window);

        virtual ~ImGuiRenderer();

        virtual void Render();

    private:
        unsigned int framebuffer;
        unsigned int textureColorbuffer;
        // ImGuiIO *io;
        float background_color[4] { 0.7137f, 0.7333f, 0.7686f, 1.0f };
    };
} // namespace ht
