#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "log/log.h"
#include "exception/exception.h"

#include <string>
#include <string_view>

struct WindowState {
    float background_color[4] { 0.7137f, 0.7333f, 0.7686f, 1.0f };
    int width { SCR_WIDTH };
    int height { SCR_HEIGHT };
    std::string title { TITLE };

private:
    constexpr static unsigned int SCR_WIDTH { 800 };
    constexpr static unsigned int SCR_HEIGHT { 600 };
    constexpr static const char *TITLE { "LearnOpenGL" };
};

WindowState windowState;

void ImGuiInit(GLFWwindow *window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext(nullptr);
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void ImGuiShutdown() {
    // 释放 ImGui 资源
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiRender() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("Edit");
    ImGui::ColorEdit4("Background color", windowState.background_color,
                      ImGuiColorEditFlags_AlphaBar);
    ImGui::End();
    ImGui::ShowDemoWindow();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

class GLFWWindowWrapper {
public:
    GLFWWindowWrapper(int width, int height, std::string_view title)
        : width { width }, height { height }, title { title } {
        if (!init()) {
            HT_CONSOLE_ERROE("Failed to initialize GLFW");
            throw ht::LocationException { "Failed to initialize GLFW" };
        }
        createWindow();
        setupCallbacks();
    }

    ~GLFWWindowWrapper() {
        glfwTerminate();
        ImGuiShutdown();
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glClearColor(windowState.background_color[0],
                         windowState.background_color[1],
                         windowState.background_color[2],
                         windowState.background_color[3]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            ImGuiRender();

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

private:
    int width {};
    int height {};
    std::string title;
    GLFWwindow *window {};

    /**
     * @brief 初始化 GLFW
     *
     * @return true 初始化成功
     * @return false 初始化失败
     */
    bool init() {
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
    void createWindow() {
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

        ImGuiInit(window);
    }

    static void ErrorCallback(int error, const char *description) {
        HT_CONSOLE_ERROE("GLFW Error {}: {}", error, description);
    }

    static void KeyCallback(GLFWwindow *window, int key, int scancode,
                            int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }

    static void FramebufferSizeCallback(GLFWwindow *window, int width,
                                        int height) {
        glViewport(0, 0, width, height);
    }

    void setupCallbacks() {
        glfwSetErrorCallback(ErrorCallback);
        glfwSetKeyCallback(window, KeyCallback);
        glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    }
};

int main() {
    GLFWWindowWrapper glfwWindow(800, 600, "GLFW Window");
    glfwWindow.mainLoop();

    return 0;
}
