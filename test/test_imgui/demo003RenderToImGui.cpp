#include <cstdint>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <string>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

int src_width { 800 };
int src_height { 800 };

int main(int argc, char *argv[]) {

#pragma region glfw init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window { glfwCreateWindow(src_width, src_height, "title",
                                          nullptr, nullptr) };
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }
#pragma endregion

#pragma region TEXTURE
    GLuint FBO, texture;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, src_width, src_height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           texture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#pragma texture

#pragma region ImGui init
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
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.66f, 0.66f, 0.66f, 1.00f);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.49f, 0.49f, 0.49f, 0.70f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.71f, 0.71f, 0.71f, 0.35f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.52f, 0.52f, 0.52f, 0.54f);
    colors[ImGuiCol_Header] = ImVec4(0.67, 0.67, 0.67, 0.31f);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

#pragma endregion

    float background_color[] { 0.7137f, 0.7333f, 0.7686f, 1.0f };

#pragma region loop
    while (!glfwWindowShouldClose(window)) {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glClearColor(background_color[0], background_color[1],
                     background_color[2], background_color[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport();

        ImGui::Begin("Edit");
        ImGui::ColorEdit4("background color", background_color,
                          ImGuiColorEditFlags_AlphaBar);
        ImGui::End();

        ImGui::Begin("Scene");
        ImGui::Image(reinterpret_cast<void *>(static_cast<intptr_t>(texture)),
                     ImVec2 { (float)src_width, (float)src_height });
        ImVec2 size = ImGui::GetWindowSize();
        src_height = size.y;
        src_width = size.x;
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
#pragma endregion
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}
