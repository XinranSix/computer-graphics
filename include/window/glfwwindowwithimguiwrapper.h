#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "window/glfwwindowwrapper.h"

#include <memory>
#include <string>
#include <string_view>
#include "imguirenderer/imguirenderer.h"

namespace ht {
    class GLFWWindowWithImGuiWrapper : public GLFWWindowWrapper {
    public:
        using GLFWWindowWrapper::GLFWWindowWrapper;

        // GLFWWindowWithImGuiWrapper(int width, int height,
        //                            std::string_view title);

        // GLFWWindowWithImGuiWrapper(int width, int height,
        //                            std::string_view title);
        // virtual ~GLFWWindowWithImGuiWrapper() override;
        virtual void run() override;

        // /**
        //  * @brief 初始化 GLFW
        //  *
        //  * @return true 初始化成功
        //  * @return false 初始化失败
        //  */
        virtual void init() override;
        virtual void setImGuiRender(ImGuiRenderer *imguiRenderer);

    protected:
        /**
         * @brief 创建 GLFWwindow，并初始化 glad
         */
        virtual void createWindow() override;
        // void setupCallbacks();

    protected:
        // int width {};
        // int height {};
        // std::string title;
        // GLFWwindow *window {};
        std::unique_ptr<ImGuiRenderer> imguiRenderer;

    protected:
        // static void ErrorCallback(int error, const char *description);
        // static void KeyCallback(GLFWwindow *window, int key, int scancode,
        //                         int action, int mods);
        // static void FramebufferSizeCallback(GLFWwindow *window, int width,
        //                                     int height);
    };

} // namespace ht
