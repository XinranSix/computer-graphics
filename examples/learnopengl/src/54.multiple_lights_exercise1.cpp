// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/fwd.hpp"
// clang-format on

#include <iostream>
#include <memory>
#include <format>
#include <string>
#include <string_view>
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
unsigned int loadTexture(const char *path);


// 全局数据

struct DirLight {
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct PointLight {
    glm::vec3 position;

    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

void render_imgui_for_dir_light(std::string_view name, DirLight &dirLight);
void render_imgui_for_point_light(std::string_view name, PointLight &pointLight);
void render_imgui_for_spot_light(std::string_view name, SpotLight &spotLight);


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

// glm::vec3 lightPos { 1.2f, 1.0f, 2.0f };

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
    Shader lightingShader("asset/shader/learnopengl/54.multiple_lights_exercise1/54.multiple_lights_exercise1.vs",
                          "asset/shader/learnopengl/54.multiple_lights_exercise1/54.multiple_lights_exercise1.fs");
    Shader lightCubeShader("asset/shader/learnopengl/54.multiple_lights_exercise1/54.light_cube.vs",
                           "asset/shader/learnopengl/54.multiple_lights_exercise1/54.light_cube.fs");
#pragma endregion

#pragma region data_and_buffer
    float background_color[] { 0.1f, 0.1f, 0.1f, 1.0f };
    bool wire_mode { false };
    glm::vec3 lightColor { 1.0f, 1.0f, 1.0f };


    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 0.0f, //
        0.5f,  -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 0.0f, //
        0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 1.0f, //
        0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 1.0f, //
        -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 1.0f, //
        -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 0.0f, //

        -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, //
        0.5f,  -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f, //
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, //
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, //
        -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f, //
        -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, //

        -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f, 0.0f, //
        -0.5f, 0.5f,  -0.5f, -1.0f, 0.0f,  0.0f,  1.0f, 1.0f, //
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f, 1.0f, //
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f, 1.0f, //
        -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f, 0.0f, //
        -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f, 0.0f, //

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, //
        0.5f,  0.5f,  -0.5f, 1.0f,  0.0f,  0.0f,  1.0f, 1.0f, //
        0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.0f, 1.0f, //
        0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.0f, 1.0f, //
        0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, //
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, //

        -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f, 1.0f, //
        0.5f,  -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  1.0f, 1.0f, //
        0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  1.0f, 0.0f, //
        0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  1.0f, 0.0f, //
        -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.0f, 0.0f, //
        -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f, 1.0f, //

        -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f, 1.0f, //
        0.5f,  0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  1.0f, 1.0f, //
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, //
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, //
        -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, //
        -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f, 1.0f  //
    };

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),     //
        glm::vec3(2.0f, 5.0f, -15.0f),   //
        glm::vec3(-1.5f, -2.2f, -2.5f),  //
        glm::vec3(-3.8f, -2.0f, -12.3f), //
        glm::vec3(2.4f, -0.4f, -3.5f),   //
        glm::vec3(-1.7f, 3.0f, -7.5f),   //
        glm::vec3(1.3f, -2.0f, -2.5f),   //
        glm::vec3(1.5f, 2.0f, -2.5f),    //
        glm::vec3(1.5f, 0.2f, -1.5f),    //
        glm::vec3(-1.3f, 1.0f, -1.5f)    //
    };

    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 0.2f, 2.0f),    //
        glm::vec3(2.3f, -3.3f, -4.0f),  //
        glm::vec3(-4.0f, 2.0f, -12.0f), //
        glm::vec3(0.0f, 0.0f, -3.0f)    //
    };

    DirLight dirLight { { -0.2f, -1.0f, -0.3f }, { 0.05f, 0.05f, 0.05f }, { 0.4f, 0.4f, 0.4f }, { 0.5f, 0.5f, 0.5f } };
    PointLight pointLight[4] { { glm::vec3(0.7f, 0.2f, 2.0f),
                                 1.0f,
                                 0.09f,
                                 0.032f,
                                 { 0.05f, 0.05f, 0.05f },
                                 { 0.8f, 0.8f, 0.8f },
                                 { 1.0f, 1.0f, 1.0f } },
                               { glm::vec3(2.3f, -3.3f, -4.0f),
                                 1.0f,
                                 0.09f,
                                 0.032f,
                                 { 0.05f, 0.05f, 0.05f },
                                 { 0.8f, 0.8f, 0.8f },
                                 { 1.0f, 1.0f, 1.0f } },
                               { glm::vec3(-4.0f, 2.0f, -12.0f),
                                 1.0f,
                                 0.09f,
                                 0.032f,
                                 { 0.05f, 0.05f, 0.05f },
                                 { 0.8f, 0.8f, 0.8f },
                                 { 1.0f, 1.0f, 1.0f } },
                               { glm::vec3(0.0f, 0.0f, -3.0f),
                                 1.0f,
                                 0.09f,
                                 0.032f,
                                 { 0.05f, 0.05f, 0.05f },
                                 { 0.8f, 0.8f, 0.8f },
                                 { 1.0f, 1.0f, 1.0f } } };
    SpotLight spotLight { camera.Position,
                          camera.Front,
                          glm::cos(glm::radians(12.5f)),
                          glm::cos(glm::radians(15.0f)),
                          1.0f,
                          0.09f,
                          0.032f,
                          { 0.0f, 0.0f, 0.0f },
                          { 1.0f, 1.0f, 1.0f },
                          { 1.0f, 1.0f, 1.0f } };


    GLuint VBO, cubeVAO, lightCubeVAO;
    glCreateVertexArrays(1, &cubeVAO);
    glCreateVertexArrays(1, &lightCubeVAO);
    glCreateBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void *>(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(lightCubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(0);

#pragma endregion

#pragma region texture
    unsigned int diffuseMap = loadTexture("asset/texture/container2.png");
    unsigned int specularMap = loadTexture("asset/texture/container2_specular.png");
    unsigned int spotlightMap = loadTexture("asset/texture/yy.png");

    // shader configuration
    // --------------------
    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);
    lightingShader.setInt("spotlightMap", 2);
    // lightingShader.setInt("spotLight.diffuse", 2);

#pragma endregion


#pragma region loop
    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
        processInput(window);
        spotLight.position = camera.Position;
        spotLight.direction = camera.Front;

        // render
        // ------
        glClearColor(background_color[0], background_color[1], background_color[2], background_color[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPolygonMode(GL_FRONT_AND_BACK, wire_mode ? GL_LINE : GL_FILL);


        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setFloat("material.shininess", 32.0f);
        // directional light
        lightingShader.setVec3("dirLight.direction", dirLight.direction);
        lightingShader.setVec3("dirLight.ambient", dirLight.ambient);
        lightingShader.setVec3("dirLight.diffuse", dirLight.diffuse);
        lightingShader.setVec3("dirLight.specular", dirLight.specular);
        // point light
        for (int i = 0; i < 4; ++i) {
            lightingShader.setVec3(std::format("pointLights[{}].position", i).c_str(), pointLight[i].position);
            lightingShader.setVec3(std::format("pointLights[{}].ambient", i).c_str(), pointLight[i].ambient);
            lightingShader.setVec3(std::format("pointLights[{}].diffuse", i).c_str(), pointLight[i].diffuse);
            lightingShader.setVec3(std::format("pointLights[{}].specular", i).c_str(), pointLight[i].specular);
            lightingShader.setFloat(std::format("pointLights[{}].constant", i).c_str(), pointLight[i].constant);
            lightingShader.setFloat(std::format("pointLights[{}].linear", i).c_str(), pointLight[i].linear);
            lightingShader.setFloat(std::format("pointLights[{}].quadratic", i).c_str(), pointLight[i].quadratic);
        }
        // spotLight
        lightingShader.setVec3("spotLight.position", spotLight.position);
        lightingShader.setVec3("spotLight.direction", spotLight.direction);
        lightingShader.setVec3("spotLight.ambient", spotLight.ambient);
        lightingShader.setVec3("spotLight.diffuse", spotLight.diffuse);
        lightingShader.setVec3("spotLight.specular", spotLight.specular);
        lightingShader.setFloat("spotLight.constant", spotLight.constant);
        lightingShader.setFloat("spotLight.linear", spotLight.linear);
        lightingShader.setFloat("spotLight.quadratic", spotLight.quadratic);
        lightingShader.setFloat("spotLight.cutOff", spotLight.cutOff);
        lightingShader.setFloat("spotLight.outerCutOff", spotLight.outerCutOff);

        auto projection { glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f,
                                           100.0f) };
        auto view { camera.GetViewMatrix() };
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // render the cube
        for (unsigned int i = 0; i < 10; i++) {
            auto model { glm::translate(glm::mat4 { 1.0f }, cubePositions[i]) };
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            lightingShader.setMat4("model", model);

            glBindVertexArray(cubeVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, diffuseMap);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, specularMap);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, spotlightMap);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        // also draw the lamp object(s)
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);

        // we now draw as many light bulbs as we have point lights.
        glBindVertexArray(lightCubeVAO);
        for (unsigned int i = 0; i < 4; i++) {
            auto model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            lightCubeShader.setMat4("model", model);
            lightCubeShader.setVec3("lightColor", lightColor);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


#pragma region imgui render
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Sceen");

        ImGui::Text("Global Setting");
        ImGui::ColorEdit4("background color", background_color);
        ImGui::Checkbox("Wire Mode", &wire_mode);
        ImGui::NewLine();
        ImGui::Text("·Light");
        ImGui::ColorEdit4("Light Color", glm::value_ptr(lightColor), ImGuiColorEditFlags_Float);
        ImGui::NewLine();
        render_imgui_for_dir_light("dir light1", dirLight);
        for (int i {}; i < 4; ++i) {
            ImGui::NewLine();
            render_imgui_for_point_light(std::format("point light{}", i), pointLight[i]);
        }
        ImGui::NewLine();
        render_imgui_for_spot_light("spot light1", spotLight);


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
    glDeleteTextures(1, &diffuseMap);

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

unsigned int loadTexture(char const *path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    stbi_set_flip_vertically_on_load(true);
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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void render_imgui_for_dir_light(std::string_view name, DirLight &dirLight) {
    ImGui::PushID(name.data());
    ImGui::Text("%s", name.data());
    ImGui::DragFloat3("direction", glm::value_ptr(dirLight.direction));
    ImGui::DragFloat3("ambient", glm::value_ptr(dirLight.ambient), 0.01, 0.0f, 1.0f);
    ImGui::DragFloat3("diffuse", glm::value_ptr(dirLight.diffuse), 0.01, 0.0f, 1.0f);
    ImGui::DragFloat3("specular", glm::value_ptr(dirLight.specular), 0.01, 0.0f, 1.0f);
    ImGui::PopID();
}

void render_imgui_for_point_light(std::string_view name, PointLight &pointLight) {
    ImGui::PushID(name.data());
    ImGui::Text("%s", name.data());
    ImGui::DragFloat3("position", glm::value_ptr(pointLight.position));
    ImGui::DragFloat("constant", &pointLight.constant);
    ImGui::DragFloat("linear", &pointLight.linear);
    ImGui::DragFloat("quadratic", &pointLight.quadratic);
    ImGui::DragFloat3("ambient", glm::value_ptr(pointLight.ambient), 0.01, 0.0f, 1.0f);
    ImGui::DragFloat3("diffuse", glm::value_ptr(pointLight.diffuse), 0.01, 0.0f, 1.0f);
    ImGui::DragFloat3("specular", glm::value_ptr(pointLight.specular), 0.01, 0.0f, 1.0f);
    ImGui::PopID();
}

void render_imgui_for_spot_light(std::string_view name, SpotLight &spotLight) {
    ImGui::PushID(name.data());
    ImGui::Text("%s", name.data());
    ImGui::DragFloat3("position", glm::value_ptr(spotLight.position));
    ImGui::DragFloat3("direction", glm::value_ptr(spotLight.direction));
    ImGui::DragFloat("cutoff", &spotLight.cutOff);
    ImGui::DragFloat("outer cutoff", &spotLight.outerCutOff);
    ImGui::DragFloat("constant", &spotLight.constant);
    ImGui::DragFloat("linear", &spotLight.linear);
    ImGui::DragFloat("quadratic", &spotLight.quadratic);
    ImGui::DragFloat3("ambient", glm::value_ptr(spotLight.ambient), 0.01, 0.0f, 1.0f);
    ImGui::DragFloat3("diffuse", glm::value_ptr(spotLight.diffuse), 0.01, 0.0f, 1.0f);
    ImGui::DragFloat3("specular", glm::value_ptr(spotLight.specular), 0.01, 0.0f, 1.0f);
    ImGui::PopID();
}
