#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imguirenderer/imguirenderer.h"

namespace ht {
    class RenderToImGuiRenderer : public ImGuiRenderer {
    public:
        using ImGuiRenderer::ImGuiRenderer;
        virtual void init() override;
        virtual ~RenderToImGuiRenderer();
        virtual void Render() override;

    protected:
        unsigned int framebuffer;
        unsigned int textureColorbuffer;
    };
} // namespace ht
