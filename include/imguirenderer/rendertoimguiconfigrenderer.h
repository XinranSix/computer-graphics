#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imguirenderer/imguirenderer.h"

namespace ht {
    class RenderToImGuiConfigRenderer : public ImGuiRenderer {
    public:
        using ImGuiRenderer::ImGuiRenderer;
        virtual void init() override;
        virtual ~RenderToImGuiConfigRenderer();
        virtual void Render() override;

    protected:
        unsigned int framebuffer;
        unsigned int textureColorbuffer;
        // // ImGuiIO *io;
        // float background_color[4] { 0.7137f, 0.7333f, 0.7686f, 1.0f };
    };
} // namespace ht
