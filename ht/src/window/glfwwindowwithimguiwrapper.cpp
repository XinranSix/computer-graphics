#include "window/glfwwindowwithimguiwrapper.h"
#include "log/log.h"
#include "exception/exception.h"

#include <string>

namespace ht {
    // GLFWWindowWithImGuiWrapper::GLFWWindowWithImGuiWrapper(
    //     int width, int height, std::string_view title)
    //     : GLFWWindowWrapper { width, height, title } {
    //     // if (!this->init()) {
    //     //     HT_CONSOLE_ERROE("Failed to initialize GLFW");
    //     //     throw ht::LocationException { "Failed to initialize GLFW" };
    //     // }
    //     // createWindow();
    //     // setupCallbacks();
    // }

    // GLFWWindowWithImGuiWrapper::~GLFWWindowWithImGuiWrapper() {
    //     glfwTerminate();
    // }

    void GLFWWindowWithImGuiWrapper::run() {
        while (!glfwWindowShouldClose(window)) {
            glClearColor(1, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
            imguiRenderer->Render();
            // 交换缓冲区和检查事件
            glfwSwapBuffers(window);
            // glfwPollEvents();
        }
    }

    void GLFWWindowWithImGuiWrapper::init() {
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

    /**
     * @brief 创建 GLFWwindow，并初始化 glad
     */
    void GLFWWindowWithImGuiWrapper::createWindow() {
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
        if (imguiRenderer == nullptr) {
            HT_CONSOLE_ERROE("ImGui not created");
            glfwTerminate();
            throw ht::LocationException { "ImGui not created" };
        }
        imguiRenderer->init();
    }

    void
    GLFWWindowWithImGuiWrapper::setImGuiRender(ImGuiRenderer *imguiRenderer) {
        this->imguiRenderer = std::unique_ptr<ImGuiRenderer> { imguiRenderer };
    }

} // namespace ht
