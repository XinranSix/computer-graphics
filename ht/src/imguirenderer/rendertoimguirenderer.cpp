#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "exception/exception.h"
#include "log/log.h"
#include "imguirenderer/rendertoimguirenderer.h"

namespace ht {

    void RenderToImGuiRenderer::init() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext(nullptr);
        ImGuiIO &io = ImGui::GetIO();
        (void)io;

        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiViewportFlags_NoDecoration;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.ConfigFlags |= ImGuiCol_DockingEmptyBg;

        ImGui_ImplGlfw_InitForOpenGL(window->getGLFWwindow(), true);
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
            HT_CONSOLE_ERROE("Framebuffer is not complete!");
            throw LocationException { "Framebuffer is not complete!" };
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    RenderToImGuiRenderer::~RenderToImGuiRenderer() {
        // 删除帧缓冲对象和纹理
        glDeleteFramebuffers(1, &framebuffer);
        glDeleteTextures(1, &textureColorbuffer);
        // // 关闭ImGui
        // ImGui_ImplOpenGL3_Shutdown();
        // ImGui_ImplGlfw_Shutdown();
        // ImGui::DestroyContext();
    }

    void RenderToImGuiRenderer::Render() {
        // 渲染到FBO
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glClearColor(window->backgroundColor[0], window->backgroundColor[1],
                     window->backgroundColor[2], window->backgroundColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // ImGui渲染准备
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport();
        ImGui::Begin("Edit");
        ImGui::ColorEdit4("background color", window->backgroundColor,
                          ImGuiColorEditFlags_AlphaBar);
        ImGui::End();
        ImGui::Begin("Scene");
        ImGui::Image(
            reinterpret_cast<void *>(static_cast<intptr_t>(textureColorbuffer)),
            ImGui::GetWindowSize());
        ImGui::End();
        ImGui::Render();
        ImGuiIO &io = ImGui::GetIO();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }
} // namespace ht
