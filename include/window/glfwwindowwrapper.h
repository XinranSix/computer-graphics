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
        virtual void run();

    public:
        GLFWWindowWrapper(GLFWWindowWrapper &) = delete;
        GLFWWindowWrapper(GLFWWindowWrapper &&) = delete;
        GLFWWindowWrapper &operator=(const GLFWWindowWrapper &) = delete;
        GLFWWindowWrapper &operator=(const GLFWWindowWrapper &&) = delete;

        /**
         * @brief 初始化 GLFW
         *
         * @return true 初始化成功
         * @return false 初始化失败
         */
        virtual void init();

    protected:
        /**
         * @brief 创建 GLFWwindow，并初始化 glad
         */
        virtual void createWindow();
        virtual void setupCallbacks();

    protected:
        int width {};
        int height {};
        std::string title;
        GLFWwindow *window {};

    public:
        float backgroundColor[4] { 0.7137f, 0.7333f, 0.7686f, 1.0f };

    public:
        virtual int getWidth() const {
            return width;
        }
        virtual int getHeight() const {
            return height;
        }
        virtual std::string getTitle() const {
            return title;
        }
        virtual GLFWwindow *getGLFWwindow() const {
            return window;
        }

    protected:
        static void ErrorCallback(int error, const char *description);
        static void KeyCallback(GLFWwindow *window, int key, int scancode,
                                int action, int mods);
        static void FramebufferSizeCallback(GLFWwindow *window, int width,
                                            int height);
    };

} // namespace ht
