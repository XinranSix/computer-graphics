#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <tuple>
// #define STB_IMAGE_IMPLEMENTATION
// #include <stb_image.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "shader/shader.h"
#include "model/model.h"
#include "image/image.h"

#include <iostream>
#include <string>
#include <string_view>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

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

class Texture {
public:
    Texture(std::string_view imagePath)
        : image { std::make_unique<ht::Image>(imagePath) },
          textureSize { std::tuple<int, int> { image->getWidth(),
                                               image->getHeight() } } {
        genTextures();
        image.reset();
    }

    ~Texture() {
        glDeleteTextures(1, &id);
    }

    void genTextures() {
        if (generated) {
            return;
        }
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if (image->getChannels() == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->getWidth(),
                         image->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE,
                         image->getData());
        } else if (image->getChannels() == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->getWidth(),
                         image->getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                         image->getData());
        }

        generated = true;
    }

    std::tuple<int, int> const &getTextureSize() const {
        return textureSize;
    }

    void use() const {
        // 绑定纹理到纹理单元
        // FIXME: 这里要根据已经加载的纹理数量计算出纹理单元
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, id);
    }

    GLuint getID() const {
        return id;
    }

private:
    GLuint id {};
    bool generated {};
    std::unique_ptr<ht::Image> image;
    std::tuple<int, int> textureSize;
};

WindowState windowState;

class ImGuiLayer {
public:
    static void ImGuiInit(GLFWwindow *window) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext(nullptr);
        ImGuiIO &io = ImGui::GetIO();
        (void)io;

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    static void ImGuiRenderPre() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    static void ImGuiRender() {
        ImGui::Begin("Edit");
        ImGui::ColorEdit4("Background color", windowState.background_color,
                          ImGuiColorEditFlags_AlphaBar);
        ImGui::End();
    }

    static void ImGuiRenderPost() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    static void ImGuiShutdown() {
        // 释放 ImGui 资源
        ImGui_ImplOpenGL3_Shutdown();
        ImGui::DestroyContext();
    }

public:
    static void ImGuiRenderImage(std::string_view name, const Texture &image) {
        ImGui::Begin(name.data());
        ImGui::Image(
            ImTextureID(reinterpret_cast<void *>(image.getID())),
            { static_cast<float>(std::get<0>(image.getTextureSize())),
              static_cast<float>(std::get<1>(image.getTextureSize())) });
        ImGui::End();
    }
};

class ImGuiRenderer {
public:
    ImGuiRenderer(GLFWwindow *window) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext(nullptr);
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
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

#pragma endregion

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
        // ImGui渲染准备
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 渲染到FBO
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 在这里添加你的OpenGL渲染代码，例如绘制一个三角形

        // 恢复默认帧缓冲
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // ImGui渲染
        ImGui::Begin("Hello, ImGui!");
        ImGui::Text("This is an ImGui window.");
        ImGui::Image((void *)(intptr_t)textureColorbuffer, ImVec2(800, 600),
                     ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

private:
    unsigned int framebuffer;
    unsigned int textureColorbuffer;
};

int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window =
        glfwCreateWindow(windowState.width, windowState.height,
                         windowState.title.c_str(), nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    ImGuiLayer::ImGuiInit(window);

    Texture image1 { "asset/image/yy.png" };
    Texture image2 { "asset/image/download.jpg" };

    while (!glfwWindowShouldClose(window)) {
        glClearColor(
            windowState.background_color[0], windowState.background_color[1],
            windowState.background_color[2], windowState.background_color[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGuiLayer::ImGuiRenderPre();
        ImGuiLayer::ImGuiRender();
        ImGuiLayer::ImGuiRenderImage("ys", image1);
        ImGuiLayer::ImGuiRenderImage("test1", image2);
        ImGuiLayer::ImGuiRenderPost();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    ImGuiLayer::ImGuiShutdown();

    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}
