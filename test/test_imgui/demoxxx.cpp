#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "shader/shader.h"
#include "model/model.h"

#include <iostream>
#include <string>
#include <string_view>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

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

class Texture2D {
public:
    Texture2D() = default;

    void genTextures() {
        if (generated) {
            return;
        }
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        generated = true;
    }

    void use() {
        genTextures();
        glBindTexture(GL_TEXTURE_2D, id);
    }

    GLuint getId() const {
        return id;
    }

private:
    GLuint id {};
    bool generated {};
};

class Image {
public:
    Image(std::string_view path) {
        // 加载并生成纹理
        texture->use();
        data = stbi_load(path.data(), &width, &height, &channels, 0);
        if (data) {
            if (channels == 3) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                             GL_UNSIGNED_BYTE, data);
            } else if (channels == 4) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                             GL_RGBA, GL_UNSIGNED_BYTE, data);
            }
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            std::cout << "Failed to load texture" << std::endl;
        }
        size = ImVec2(width, height);
        stbi_image_free(data);
    }

    ~Image() {
        stbi_image_free(data);
        data = nullptr;
    }

    GLuint getTextureId() const {
        return texture->getId();
    }

    void genTextures() {
        if (texture == nullptr) {
            texture = std::make_unique<Texture2D>();
        }
        texture->genTextures();
    }

private:
    std::unique_ptr<Texture2D> texture;

    int width {};
    int height {};
    int channels {};
    unsigned char *data {};

public:
    ImVec2 uv0 = ImVec2(0.0f, 0.0f);
    ImVec2 uv1 = ImVec2(1.0f, 1.0f);
    ImVec2 size;
};

WindowState windowState;

void ImGuiRenderImage(std::string_view name, const Image &image) {
    ImGui::Begin(name.data());
    ImGui::Image(ImTextureID(reinterpret_cast<void *>(image.getTextureId())),
                 image.size);
    ImGui::End();
}

void ImGuiInit(GLFWwindow *window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext(nullptr);
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void ImGuiRenderPre() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiRenderPost() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiRender() {
    ImGui::Begin("Edit");
    ImGui::ColorEdit4("Background color", windowState.background_color,
                      ImGuiColorEditFlags_AlphaBar);
    ImGui::End();
}

void ImGuiShutdown() {
    // 释放 ImGui 资源
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}

int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

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

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    ImGuiInit(window);

    Image image1 { "asset/image/yy.png" };
    Image image2 { "asset/image/download.jpg" };

    while (!glfwWindowShouldClose(window)) {
        glClearColor(
            windowState.background_color[0], windowState.background_color[1],
            windowState.background_color[2], windowState.background_color[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGuiRenderPre();
        ImGuiRender();
        ImGuiRenderImage("ys", image1);
        ImGuiRenderImage("test1", image2);
        ImGuiRenderPost();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    ImGuiShutdown();

    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}
