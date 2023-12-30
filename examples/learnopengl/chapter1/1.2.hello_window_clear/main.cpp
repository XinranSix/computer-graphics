#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "shader/shader.h"
#include "model/model.h"

#include <iostream>
#include <string>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// settings
/* constexpr unsigned int SCR_WIDTH = 800;
constexpr unsigned int SCR_HEIGHT = 600;
 */

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

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
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

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    /*    IMGUI_CHECKVERSION();
       ImGui::CreateContext(nullptr);
       ImGuiIO &io = ImGui::GetIO();
       (void)io;

       ImGui_ImplGlfw_InitForOpenGL(window, true);
       ImGui_ImplOpenGL3_Init("#version 330");
    */
    ImGuiInit(window);

    // float background_color[] { 0.7137f, 0.7333f, 0.7686f, 1.0f };

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        glClearColor(
            windowState.background_color[0], windowState.background_color[1],
            windowState.background_color[2], windowState.background_color[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGuiRender();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();

    /*     // 释放ImGui资源
        ImGui_ImplOpenGL3_Shutdown();
        ImGui::DestroyContext(); */
    ImGuiShutdown();

    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width
    // and height will be significantly larger than specified on retina
    // displays.
    glViewport(0, 0, width, height);
}
