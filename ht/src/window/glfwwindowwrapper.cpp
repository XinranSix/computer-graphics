#include "window/glfwwindowwrapper.h"
#include "log/log.h"
#include "exception/exception.h"

#include <string>

namespace ht {
    GLFWWindowWrapper::GLFWWindowWrapper(int width, int height,
                                         std::string_view title)
        : width { width }, height { height }, title { title } {
        if (!init()) {
            HT_CONSOLE_ERROE("Failed to initialize GLFW");
            throw ht::LocationException { "Failed to initialize GLFW" };
        }
        createWindow();
        setupCallbacks();
    }

    GLFWWindowWrapper::~GLFWWindowWrapper() {
        glfwTerminate();
    }

    void GLFWWindowWrapper::mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            // 处理输入事件
            glfwPollEvents();
            // 渲染纹理
            // texture.Bind();
            // 在这里添加你的OpenGL渲染代码，例如绘制一个三角形
            // glClearColor(
            //     windowState.background_color[0],
            //     windowState.background_color[1],
            //     windowState.background_color[2],
            //     windowState.background_color[3]);
            // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            // 渲染ImGui
            // imguiRenderer->Render();
            // 交换缓冲区和检查事件
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    bool GLFWWindowWrapper::init() {
        if (!glfwInit()) {
            return false;
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
        return true;
    }

    /**
     * @brief 创建 GLFWwindow，并初始化 glad
     */
    void GLFWWindowWrapper::createWindow() {
        window =
            glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (window == nullptr) {
            HT_CONSOLE_ERROE("Failed to create GLFW window");
            glfwTerminate();
            // std::exit(EXIT_FAILURE);
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

        // imguiRenderer = std::make_unique<ImGuiRenderer>(window);
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

    void GLFWWindowWrapper::setupCallbacks() {
        glfwSetErrorCallback(ErrorCallback);
        glfwSetKeyCallback(window, KeyCallback);
        glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    }

} // namespace ht
