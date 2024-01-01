#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imguirenderer/imguirenderer.h"

namespace ht {
    ImGuiRenderer::ImGuiRenderer(GLFWWindowWrapper *window)
        : window { window } {
    }

    void ImGuiRenderer::init() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext(nullptr);
        ImGuiIO &io = ImGui::GetIO();
        (void)io;

        ImGui_ImplGlfw_InitForOpenGL(window->getGLFWwindow(), true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    ImGuiRenderer::~ImGuiRenderer() {
        // 关闭ImGui
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiRenderer::Render() {
        // ImGui渲染准备
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Edit");
        ImGui::ColorEdit4("background color", window->backgroundColor,
                          ImGuiColorEditFlags_AlphaBar);
        ImGui::End();
        ImGui::Begin("Scene");
        ImGui::End();
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        ImGuiIO &io = ImGui::GetIO();
        if ((io).ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }
} // namespace ht
