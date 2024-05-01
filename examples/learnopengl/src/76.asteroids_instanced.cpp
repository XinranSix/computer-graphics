// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <stb_image.h>
#include <vector>

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/fwd.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "shader_m.h"
#include "camera.h"
#include "model_e.h"
// #include 


void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void window_focus(GLFWwindow *window, int focused);
GLuint loadTexture(const char *path);
GLuint loadCubeMap(std::vector<std::string> faces);

// 全局数据
constexpr unsigned int SCR_WIDTH = 800;
constexpr unsigned int SCR_HEIGHT = 600;

bool reload_shader {};

Camera camera { glm::vec3(0.0f, 0.0f, 55.0f) };
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
    glfwSetWindowFocusCallback(window, window_focus);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glEnable(GL_DEPTH_TEST);
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
    auto asteroidShader { std::make_shared<Shader>("asset/shader/learnopengl/76.asteroids_instanced/76.asteroids.vs",
                                                   "asset/shader/learnopengl/76.asteroids_instanced/76.asteroids.fs") };
    auto planetShader { std::make_shared<Shader>("asset/shader/learnopengl/76.asteroids_instanced/76.planet.vs",
                                                 "asset/shader/learnopengl/76.asteroids_instanced/76.planet.fs") };

    std::vector<std::shared_ptr<Shader>> shaders { asteroidShader, planetShader };
#pragma endregion

#pragma region data_and_buffer
    float background_color[] { 0.1f, 0.1f, 0.1f, 1.0f };

    Model rock { "asset/model/rock/rock.obj" };
    Model planet { "asset/model/planet/planet.obj" };

    // generate a large list of semi-random model transformation matrices
    // ------------------------------------------------------------------
    unsigned int amount = 100000;
    glm::mat4 *modelMatrices;
    modelMatrices = new glm::mat4[amount];
    srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed
    float radius = 150.0;
    float offset = 25.0f;
    for (unsigned int i = 0; i < amount; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        // 1. translation: displace along circle with 'radius' in range [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. scale: Scale between 0.05 and 0.25f
        float scale = static_cast<float>((rand() % 20) / 100.0 + 0.05);
        model = glm::scale(model, glm::vec3(scale));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = static_cast<float>((rand() % 360));
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. now add to list of matrices
        modelMatrices[i] = model;
    }

    // configure instanced array
    // -------------------------
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

    // set transformation matrices as an instance vertex attribute (with divisor 1)
    // note: we're cheating a little by taking the, now publicly declared, VAO of the model's mesh(es) and adding
    // new vertexAttribPointers normally you'd want to do this in a more organized fashion, but for learning
    // purposes this will do.
    // -----------------------------------------------------------------------------------------------------------------------------------
    for (unsigned int i = 0; i < rock.meshes.size(); i++) {
        unsigned int VAO = rock.meshes[i].VAO;
        glBindVertexArray(VAO);
        // set attribute pointers for matrix (4 times vec4)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }

#pragma endregion

#pragma region uniform_buffer_object

#pragma endregion

#pragma region framebuffer
#pragma endregion

#pragma region texture

#pragma endregion

#pragma region loop
    while (!glfwWindowShouldClose(window)) {
        if (reload_shader) {
            for (auto &shader : shaders) {
                // Shader::reload(*shader);
            }
            reload_shader = false;
        }

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        glClearColor(background_color[0], background_color[1], background_color[2], background_color[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection =
            glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        asteroidShader->use();
        asteroidShader->setMat4("projection", projection);
        asteroidShader->setMat4("view", view);
        planetShader->use();
        planetShader->setMat4("projection", projection);
        planetShader->setMat4("view", view);

        // draw planet
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
        model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
        planetShader->setMat4("model", model);
        planet.Draw(*planetShader);

        // draw meteorites
        asteroidShader->use();
        asteroidShader->setInt("texture_diffuse1", 0);
        glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, rock.textures_loaded[0].id);
        for (unsigned int i = 0; i < rock.meshes.size(); i++) {
            glBindVertexArray(rock.meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(rock.meshes[i].indices.size()),
                                    GL_UNSIGNED_INT, 0, amount);
            glBindVertexArray(0);
        }


#pragma region imgui render
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Sceen");

        ImGui::Text("Global Setting");
        ImGui::ColorEdit4("background color", background_color);
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

void window_focus(GLFWwindow *window, int focused) {
    if (focused) {
        reload_shader = true;
    } else {
        reload_shader = false;
    }
}

GLuint loadCubeMap(std::vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                         data);
            stbi_image_free(data);
        } else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
