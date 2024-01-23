// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "shader_s.h"
#include "camera.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);


// 全局数据
constexpr unsigned int SCR_WIDTH = 800;
constexpr unsigned int SCR_HEIGHT = 600;

Camera camera { glm::vec3(0.0f, 0.0f, 3.0f) };
float lastX { SCR_WIDTH / 2.0f };
float lastY { SCR_HEIGHT / 2.0f };
bool firstMouse { true };
bool isInWindow { true };

// timing
float deltaTime {}; // time between current frame and last frame
float lastFrame {};

glm::vec3 lightPos { 1.2f, 1.0f, 2.0f };

int main(int argc, char *argv[]) {
#pragma region opengl init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

#pragma endregion

#pragma region imgui init
    IMGUI_CHECKVERSION();
    ImGui::CreateContext(nullptr);
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiViewportFlags_NoDecoration;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiCol_DockingEmptyBg;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");
#pragma endregion

#pragma region shader
    Shader lightingShader("asset/shader/learnopengl/35.basic_lighting_specular/35.basic_lighting_specular.vs",
                          "asset/shader/learnopengl/35.basic_lighting_specular/35.basic_lighting_specular.fs");
    Shader lightCubeShader("asset/shader/learnopengl/35.basic_lighting_specular/35.light_cube.vs",
                           "asset/shader/learnopengl/35.basic_lighting_specular/35.light_cube.fs");
#pragma endregion

#pragma region data_and_buffer
    float background_color[] { 0.1f, 0.1f, 0.1f, 1.0f };
    bool wire_mode { false };
    glm::vec3 lightColor { 1.0f, 1.0f, 1.0f };
    glm::vec3 objectColor { 1.0f, 0.5f, 0.31f };
    float ambientStrength { 0.1f };
    float specularStrength { 0.5f };
    float shininess { 32.0f };

    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, //
        0.5f,  -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, //
        0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, //
        0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, //
        -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, //
        -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, //

        -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f, //
        0.5f,  -0.5f, 0.5f,  0.0f,  0.0f,  1.0f, //
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, //
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, //
        -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f, //
        -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f, //

        -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f, //
        -0.5f, 0.5f,  -0.5f, -1.0f, 0.0f,  0.0f, //
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f, //
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f, //
        -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f, //
        -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f, //

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, //
        0.5f,  0.5f,  -0.5f, 1.0f,  0.0f,  0.0f, //
        0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f, //
        0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f, //
        0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f, //
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, //

        -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f, //
        0.5f,  -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f, //
        0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f, //
        0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f, //
        -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f, //
        -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f, //

        -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f, //
        0.5f,  0.5f,  -0.5f, 0.0f,  1.0f,  0.0f, //
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, //
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, //
        -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f, //
        -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f  //
    };

    GLuint VBO, cubeVAO, lightCubeVAO;
    glCreateVertexArrays(1, &cubeVAO);
    glCreateVertexArrays(1, &lightCubeVAO);
    glCreateBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(lightCubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

#pragma endregion

#pragma region texture

#pragma endregion


#pragma region loop
    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(background_color[0], background_color[1], background_color[2], background_color[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPolygonMode(GL_FRONT_AND_BACK, wire_mode ? GL_LINE : GL_FILL);


        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        lightingShader.use();
        lightingShader.setVec3("objectColor", objectColor);
        lightingShader.setVec3("lightColor", lightColor);
        lightingShader.setVec3("lightPos", lightPos);
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setFloat("ambientStrength", ambientStrength);
        lightingShader.setFloat("specularStrength", specularStrength);
        lightingShader.setFloat("shininess", shininess);


        auto projection { glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f,
                                           100.0f) };
        auto view { camera.GetViewMatrix() };
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        auto model { glm::mat4(1.0f) };
        lightingShader.setMat4("model", model);
        // render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // also draw the lamp object
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lightCubeShader.setMat4("model", model);
        lightCubeShader.setVec3("lightColor", lightColor);
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

#pragma region imgui render
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Sceen");

        ImGui::Text("Global Setting");
        ImGui::ColorEdit4("background color", background_color);
        ImGui::Checkbox("Wire Mode", &wire_mode);
        ImGui::NewLine();
        ImGui::ColorEdit4("Object Color", glm::value_ptr(objectColor), ImGuiColorEditFlags_Float);
        ImGui::ColorEdit4("Light Color", glm::value_ptr(lightColor), ImGuiColorEditFlags_Float);
        ImGui::DragFloat("ambient strength", &ambientStrength, 0.0001f, 0.01f, 1.0f);
        ImGui::DragFloat("specular strength", &specularStrength, 0.0001f, 0.01f, 1.0f);
        ImGui::DragFloat("shininess", &shininess, 1.0f, 0.0f, 256.0f);

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

#pragma endregion
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
#pragma endregion

#pragma region shutdown
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
#pragma endregion

    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        isInWindow = true;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        isInWindow = false;
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
    if (isInWindow) {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

