#include "window/glfwwindowwrapper.h"
#include "log/log.h"
#include "exception/exception.h"

#include <string>

namespace ht {
    GLFWWindowWrapper::GLFWWindowWrapper(int width, int height,
                                         std::string_view title)
        : width { width }, height { height }, title { title } {
    }

    GLFWWindowWrapper::~GLFWWindowWrapper() {
        glfwTerminate();
    }

    void GLFWWindowWrapper::run() {
        while (!glfwWindowShouldClose(window)) {
            glClearColor(backgroundColor[0], backgroundColor[1],
                         backgroundColor[2], backgroundColor[3]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            // 处理输入事件
            glfwPollEvents();
            // 在这里添加你的OpenGL渲染代码，例如绘制一个三角形

            // 交换缓冲区和检查事件
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    void GLFWWindowWrapper::init() {
        if (!glfwInit()) {
            HT_CONSOLE_ERROE("Failed to initialize GLFW");
            throw ht::LocationException { "Failed to initialize GLFW" };
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
        createWindow();
        setupCallbacks();
    }

    void GLFWWindowWrapper::setupCallbacks() {
        glfwSetErrorCallback(ErrorCallback);
        glfwSetKeyCallback(window, KeyCallback);
        glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    }

    void GLFWWindowWrapper::createWindow() {
        window =
            glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (window == nullptr) {
            HT_CONSOLE_ERROE("Failed to create GLFW window");
            glfwTerminate();
            throw ht::LocationException { "Failed to create GLFW window" };
        }
        glfwMakeContextCurrent(window);

        // 使用 glad 加载 OpenGL 函数指针
        if (!gladLoadGLLoader(
                reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
            HT_CONSOLE_ERROE("Failed to initialize GLAD");
            glfwTerminate();
            throw ht::LocationException { "Failed to initialize GLAD" };
        }
    }

    void GLFWWindowWrapper::ErrorCallback(int error, const char *description) {
        HT_CONSOLE_ERROE("GLFW Error {}: {}", error, description);
    }

    void GLFWWindowWrapper::KeyCallback(GLFWwindow *window, int key,
                                        int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }

    void GLFWWindowWrapper::FramebufferSizeCallback(GLFWwindow *window,
                                                    int width, int height) {
        glViewport(0, 0, width, height);
    }

} // namespace ht
