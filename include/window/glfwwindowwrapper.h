#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <string_view>

namespace ht {
    class GLFWWindowWrapper {
    public:
        GLFWWindowWrapper(int width, int height, std::string_view title);
        virtual ~GLFWWindowWrapper();
        virtual void mainLoop();

    public:
        GLFWWindowWrapper(GLFWWindowWrapper &) = delete;
        GLFWWindowWrapper(GLFWWindowWrapper &&) = delete;
        GLFWWindowWrapper &operator=(const GLFWWindowWrapper &) = delete;
        GLFWWindowWrapper &operator=(const GLFWWindowWrapper &&) = delete;

    private:
        /**
         * @brief 初始化 GLFW
         *
         * @return true 初始化成功
         * @return false 初始化失败
         */
        bool init();
        /**
         * @brief 创建 GLFWwindow，并初始化 glad
         */
        void createWindow();
        void setupCallbacks();

    private:
        int width {};
        int height {};
        std::string title;
        GLFWwindow *window {};

    private:
        static void ErrorCallback(int error, const char *description);
        static void KeyCallback(GLFWwindow *window, int key, int scancode,
                                int action, int mods);
        static void FramebufferSizeCallback(GLFWwindow *window, int width,
                                            int height);
    };

} // namespace ht
