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
#include <iostream>

#include <memory>
#include <string>
#include <string_view>

class ImGuiRenderer {
public:
    ImGuiRenderer(GLFWwindow *window) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext(nullptr);
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        this->io = &io;
        io.Fonts->AddFontFromFileTTF(
            "./asset/font/MSYH.TTC", 20, nullptr,
            io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiViewportFlags_NoDecoration;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.ConfigFlags |= ImGuiCol_DockingEmptyBg;

        ImGui::StyleColorsLight();
        ImGuiStyle &style = ImGui::GetStyle();

        style.WindowRounding = 12;
        style.ChildRounding = 12;
        style.FrameRounding = 12;
        style.PopupRounding = 6;
        style.ScrollbarRounding = 8;
        style.GrabRounding = 12;
        style.TabRounding = 8;

        ImVec4 *colors { style.Colors };
        colors[ImGuiCol_BorderShadow] = ImVec4(0.66f, 0.66f, 0.66f, 0.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.47f, 0.47f, 0.47f, 0.40f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.79f, 0.79f, 0.79f, 0.67f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.65f, 0.65f, 0.65f, 0.40f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.66f, 0.66f, 0.66f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.70f, 0.70f, 0.70f, 0.00f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.60f, 0.60f, 0.60f, 0.78f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.75, 0.75, 0.75, 1.00F);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.25f, 0.25f, 0.25f, 0.20f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.36, 0.36, 0.36, 0.67f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.74, 0.74, 0.74, 0.95f);
        colors[ImGuiCol_Tab] = ImVec4(0.64f, 0.64f, 0.64f, 0.86f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.24f, 0.24f, 0.24f, 0.80f);
        colors[ImGuiCol_TabActive] = ImVec4(0.68f, 0.72f, 0.76f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive] =
            ImVec4(0.66f, 0.66f, 0.66f, 1.00f);
        colors[ImGuiCol_DockingPreview] = ImVec4(0.49f, 0.49f, 0.49f, 0.70f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.71f, 0.71f, 0.71f, 0.35f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.52f, 0.52f, 0.52f, 0.54f);
        colors[ImGuiCol_Header] = ImVec4(0.67, 0.67, 0.67, 0.31f);

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        // 创建帧缓冲对象（FBO）
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        // 创建颜色附件纹理
        glGenTextures(1, &textureColorbuffer);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, textureColorbuffer, 0);

        // 检查帧缓冲完整性
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
            GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Framebuffer is not complete!\n";
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    ~ImGuiRenderer() {
        // 删除帧缓冲对象和纹理
        glDeleteFramebuffers(1, &framebuffer);
        glDeleteTextures(1, &textureColorbuffer);
        // 关闭ImGui
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void Render() {
        // 渲染到FBO
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glClearColor(background_color[0], background_color[1],
                     background_color[2], background_color[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // ImGui渲染准备
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport();
        ImGui::Begin("Edit");
        ImGui::ColorEdit4("background color", background_color,
                          ImGuiColorEditFlags_AlphaBar);
        ImGui::End();
        ImGui::Begin("Scene");
        ImGui::Image(
            reinterpret_cast<void *>(static_cast<intptr_t>(textureColorbuffer)),
            ImVec2 { 800.0f, 600.0f });
        // ImVec2 size = ImGui::GetWindowSize();
        // src_height = size.y;
        // src_width = size.x;
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if ((*io).ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

private:
    unsigned int framebuffer;
    unsigned int textureColorbuffer;
    ImGuiIO *io;
    float background_color[4] { 0.7137f, 0.7333f, 0.7686f, 1.0f };
};

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
    }

    void mainLoop() {
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
            imguiRenderer->Render();
            // 交换缓冲区和检查事件
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

private:
    int width {};
    int height {};
    std::string title;
    GLFWwindow *window {};
    std::unique_ptr<ImGuiRenderer> imguiRenderer;

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

        imguiRenderer = std::make_unique<ImGuiRenderer>(window);
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
