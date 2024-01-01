#pragma once

#include "window/glfwwindowwrapper.h"
#include "imguirenderer/imguirenderer.h"

#include <memory>

namespace ht {
    class GLFWWindowWithImGuiWrapper : public GLFWWindowWrapper {
    public:
        using GLFWWindowWrapper::GLFWWindowWrapper;

        virtual void run() override;

        virtual void setImGuiRender(ImGuiRenderer *imguiRenderer);

    protected:
        /**
         * @brief 创建 GLFWwindow，并初始化 glad
         */
        virtual void createWindow() override;

    protected:
        std::unique_ptr<ImGuiRenderer> imguiRenderer;
    };

} // namespace ht
