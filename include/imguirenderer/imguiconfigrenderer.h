#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "window/glfwwindowwrapper.h"
#include "imguirenderer/imguirenderer.h"

namespace ht {
    class ImGuiConfigRenderer : public ImGuiRenderer {
    public:
        using ImGuiRenderer::ImGuiRenderer;
        virtual void init();
        virtual void Render();
    };
} // namespace ht
