// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

#include <iostream>
#include <string>
#include <stb_image.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "shader_m.h"
#include "camera.h"
#include "model.h"


void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
GLuint loadTexture(const char *path);

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

int main(int argc, char *argv[]) {
#pragma region opengl init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Model Loading", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
    Shader shader { "asset/shader/learnopengl/60.blending_sort/60.blending_sort.vs",
                    "asset/shader/learnopengl/60.blending_sort/60.blending_sort.fs" };
#pragma endregion

#pragma region data_and_buffer
    float background_color[] { 0.1f, 0.1f, 0.1f, 1.0f };
    bool wire_mode { false };

    GLfloat cubeVertices[] = {
        // positions          // texture Coords
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, //
        0.5f,  -0.5f, -0.5f, 1.0f, 0.0f, //
        0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, //
        0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, //
        -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, //
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, //

        -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, //
        0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, //
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, //
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, //
        -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, //
        -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, //

        -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, //
        -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f, //
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, //
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, //
        -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, //
        -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, //

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, //
        0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, //
        0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, //
        0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, //
        0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, //
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, //

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, //
        0.5f,  -0.5f, -0.5f, 1.0f, 1.0f, //
        0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, //
        0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, //
        -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, //
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, //

        -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, //
        0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, //
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, //
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, //
        -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, //
        -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f  //
    };
    GLfloat planeVertices[] = {
        // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture
        // wrapping mode). this will cause the floor texture to repeat)
        5.0f,  -0.5f, 5.0f,  2.0f, 0.0f, //
        -5.0f, -0.5f, 5.0f,  0.0f, 0.0f, //
        -5.0f, -0.5f, -5.0f, 0.0f, 2.0f, //

        5.0f,  -0.5f, 5.0f,  2.0f, 0.0f, //
        -5.0f, -0.5f, -5.0f, 0.0f, 2.0f, //
        5.0f,  -0.5f, -5.0f, 2.0f, 2.0f  //
    };
    GLfloat transparentVertices[] = {
        // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
        0.0f, 0.5f,  0.0f, 0.0f, 0.0f, //
        0.0f, -0.5f, 0.0f, 0.0f, 1.0f, //
        1.0f, -0.5f, 0.0f, 1.0f, 1.0f, //

        0.0f, 0.5f,  0.0f, 0.0f, 0.0f, //
        1.0f, -0.5f, 0.0f, 1.0f, 1.0f, //
        1.0f, 0.5f,  0.0f, 1.0f, 0.0f  //
    };
    std::vector<glm::vec3> windows;
    windows.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
    windows.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
    windows.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
    windows.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
    windows.push_back(glm::vec3(0.5f, 0.0f, -0.6f));

    // cube VAO
    GLuint cubeVAO, cubeVBO;
    glCreateVertexArrays(1, &cubeVAO);
    glCreateBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    glBindVertexArray(0);
    // plane VAO
    GLuint planeVAO, planeVBO;
    glCreateVertexArrays(1, &planeVAO);
    glCreateBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    glBindVertexArray(0);
    // vegetation VAO
    GLuint vegetationVAO, vegetationVBO;
    glCreateVertexArrays(1, &vegetationVAO);
    glCreateBuffers(1, &vegetationVBO);
    glBindVertexArray(vegetationVAO);
    glBindBuffer(GL_ARRAY_BUFFER, vegetationVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), &transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    glBindVertexArray(0);
#pragma endregion

#pragma region texture
    // load textures
    // -------------
    GLuint cubeTexture = loadTexture("asset/texture/marble.jpg");
    // GLuint floorTexture = loadTexture("asset/texture/metal.png");
    GLuint floorTexture = loadTexture("asset/texture/q3sky/zipfiles/ely_peaks/ely_peaks/peaks_bk.tga");
    GLuint windowTexture = loadTexture("asset/texture/window.png");
    // shader configuration
    // --------------------
    shader.use();
    shader.setInt("texture1", 0);
#pragma endregion

#pragma region loop
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        // -----
        processInput(window);
        // render
        // ------
        glClearColor(background_color[0], background_color[1], background_color[2], background_color[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPolygonMode(GL_FRONT_AND_BACK, wire_mode ? GL_LINE : GL_FILL);

        std::map<float, glm::vec3> sorted;
        for (unsigned int i = 0; i < windows.size(); i++) {
            float distance = glm::length(camera.Position - windows[i]);
            sorted[distance] = windows[i];
        }

        shader.use();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection =
            glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        // cubes
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // floor
        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        shader.setMat4("model", glm::mat4(1.0f));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(vegetationVAO);
        glBindTexture(GL_TEXTURE_2D, windowTexture);
        /*  for (unsigned int i = 0; i < windows.size(); i++) {
             model = glm::mat4(1.0f);
             model = glm::translate(model, windows[i]);
             shader.setMat4("model", model);
             glDrawArrays(GL_TRIANGLES, 0, 6);
         } */
        for (auto it = sorted.rbegin(); it != sorted.rend(); ++it) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, it->second);
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        glBindVertexArray(0);

#pragma region imgui render
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Sceen");

        ImGui::Text("Global Setting");
        ImGui::ColorEdit4("background color", background_color);
        ImGui::Checkbox("Wire Mode", &wire_mode);
        ImGui::NewLine();

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
    /*     glDeleteVertexArrays(1, &cubeVAO);
        glDeleteVertexArrays(1, &lightCubeVAO);
        glDeleteBuffers(1, &VBO);
        glDeleteTextures(1, &diffuseMap); */

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

GLuint loadTexture(const char *path) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        if (nrComponents == 4) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
