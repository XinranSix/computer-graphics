#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "imguirenderer/imguiconfigrenderer.h"

namespace ht {

    void ImGuiConfigRenderer::init() {
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

        ImGui_ImplGlfw_InitForOpenGL(window->getGLFWwindow(), true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    void ImGuiConfigRenderer::Render() {
        // glClearColor(window->backgroundColor[0], window->backgroundColor[1],
        //              window->backgroundColor[2], window->backgroundColor[3]);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport();
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
