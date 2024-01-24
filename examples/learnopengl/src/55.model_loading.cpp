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

void render_imgui_for_dir_light(std::string_view name, DirLight &dirLight);
void render_imgui_for_point_light(std::string_view name, PointLight &pointLight);
// void render_imgui_for_spot_light(std::string_view name, SpotLight &spotLight);

constexpr unsigned int SCR_WIDTH = 1920;
constexpr unsigned int SCR_HEIGHT = 1080;

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

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(false);
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
    Shader ourShader { "asset/shader/learnopengl/55.model_loading/55.model_loading.vs",
                       "asset/shader/learnopengl/55.model_loading/55.model_loading.fs" };
    Shader lightCubeShader("asset/shader/learnopengl/55.model_loading/55.light_cube.vs",
                           "asset/shader/learnopengl/55.model_loading/55.light_cube.fs");
#pragma endregion

#pragma region data_and_buffer
    float background_color[] { 0.1f, 0.1f, 0.1f, 1.0f };
    bool wire_mode { false };

    GLfloat vertices[] = {
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

    Model ourModel1("asset/model/nanosuit/nanosuit.obj");
    // Model ourModel("c:/Users/yaojie/Desktop/untitled.obj");
    Model ourModel2("asset/model/GenshinImpact/Ayaka/Ayaka model.pmx");
    // Model ourModel("asset/model/dragon.off");
    Model ourModel3("asset/model/Xenoblade/Pneuma/pneuma.pmx"); // 小绿
    // Model ourModel("asset/model/bastion-ganymede-overwatch/source/Bastion_Final.obj");

    GLuint VBO, lightCubeVAO;

    glCreateVertexArrays(1, &lightCubeVAO);
    glCreateBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(lightCubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(0);
#pragma endregion

#pragma region texture
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
        ourShader.use();

        {
            ourShader.setVec3("viewPos", camera.Position);
            // ourShader.setFloat("material.shininess", 32.0f);
            // directional light
            ourShader.setVec3("dirLight.direction", dirLight.direction);
            ourShader.setVec3("dirLight.ambient", dirLight.ambient);
            ourShader.setVec3("dirLight.diffuse", dirLight.diffuse);
            ourShader.setVec3("dirLight.specular", dirLight.specular);
            // point light
            for (int i = 0; i < 4; ++i) {
                ourShader.setVec3(std::format("pointLights[{}].position", i).c_str(), pointLight[i].position);
                ourShader.setVec3(std::format("pointLights[{}].ambient", i).c_str(), pointLight[i].ambient);
                ourShader.setVec3(std::format("pointLights[{}].diffuse", i).c_str(), pointLight[i].diffuse);
                ourShader.setVec3(std::format("pointLights[{}].specular", i).c_str(), pointLight[i].specular);
                ourShader.setFloat(std::format("pointLights[{}].constant", i).c_str(), pointLight[i].constant);
                ourShader.setFloat(std::format("pointLights[{}].linear", i).c_str(), pointLight[i].linear);
                ourShader.setFloat(std::format("pointLights[{}].quadratic", i).c_str(), pointLight[i].quadratic);
            }
        }

        // view/projection transformations
        glm::mat4 projection =
            glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model =
            glm::translate(model, glm::vec3(0.0f, 0.0f, -10.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f)); // it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        ourModel1.Draw(ourShader);
        model = glm::translate(model, glm::vec3(10.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", model);
        model = glm::translate(model, glm::vec3(30.0f, 0.0f, 0.0f));
        ourModel2.Draw(ourShader);
        ourShader.setMat4("model", model);
        ourModel3.Draw(ourShader);

        { // also draw the lamp object(s)
            lightCubeShader.use();
            lightCubeShader.setMat4("projection", projection);
            lightCubeShader.setMat4("view", view);

            // we now draw as many light bulbs as we have point lights.
            glBindVertexArray(lightCubeVAO);
            for (unsigned int i = 0; i < 4; i++) {
                auto model = glm::mat4(1.0f);
                model = glm::translate(model, pointLight[i].position);
                model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
                lightCubeShader.setMat4("model", model);
                // lightCubeShader.setVec3("lightColor", lightColor);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
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
        render_imgui_for_dir_light("dir light1", dirLight);
        for (int i {}; i < 4; ++i) {
            ImGui::NewLine();
            render_imgui_for_point_light(std::format("point light{}", i), pointLight[i]);
        }

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
