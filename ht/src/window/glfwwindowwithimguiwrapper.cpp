#include "window/glfwwindowwithimguiwrapper.h"
#include "log/log.h"
#include "exception/exception.h"

namespace ht {

    void GLFWWindowWithImGuiWrapper::run() {
        while (!glfwWindowShouldClose(window)) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            // 处理输入事件
            glfwPollEvents();
            // 在这里添加你的OpenGL渲染代码，例如绘制一个三角形

            imguiRenderer->Render();
            // 交换缓冲区和检查事件
            glfwSwapBuffers(window);
        }
    }

    void GLFWWindowWithImGuiWrapper::createWindow() {
        GLFWWindowWrapper::createWindow();
        if (imguiRenderer == nullptr) {
            HT_CONSOLE_ERROE("ImGuiRenderer not initialized");
            throw LocationException { "ImGuiRenderer not initialized" };
            glfwTerminate();
        }
        imguiRenderer->init();
    }

    void
    GLFWWindowWithImGuiWrapper::setImGuiRender(ImGuiRenderer *imguiRenderer) {
        this->imguiRenderer = std::unique_ptr<ImGuiRenderer> { imguiRenderer };
    }

} // namespace ht
