#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

#include <iostream>
#include <memory>
#include <string>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <string_view>

class Application {
public:
    Application(std::string_view title = "LearnOpenGL", int width = 800, int height = 600)
        : title_ { title }, width_ { width }, height_ { height } {
    }

    virtual ~Application() {
        shutdown();
    }

    virtual void init() {
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
        window_ = glfwCreateWindow(width_, height_, title_.c_str(), nullptr, nullptr);
        if (window_ == nullptr) {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }
        glfwMakeContextCurrent(window_);
        glfwSetFramebufferSizeCallback(window_, onFrameBufferSizeCallback);

        // glad: load all OpenGL function pointers
        // ---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return;
        }
    }

    virtual void render() {
        // input
        // -----
        processInput(window_);

        // render
        // ------
        glClearColor(background_color[0], background_color[1], background_color[2], background_color[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window_);
        glfwPollEvents();
    }

    virtual void run() {
        init();
        while (!glfwWindowShouldClose(window_)) {
            render();
        }
    }

    virtual void shutdown() {
        glfwDestroyWindow(window_);
        glfwTerminate();
    }

protected:
    std::string title_;
    int width_;
    int height_;
    GLFWwindow *window_;
    float background_color[4] { 0.7137f, 0.7333f, 0.7686f, 1.0f };


protected:
    static void onFrameBufferSizeCallback(GLFWwindow *window, int width, int height) {
        // make sure the viewport matches the new window dimensions; note that width and
        // height will be significantly larger than specified on retina displays.
        glViewport(0, 0, width, height);
    }

    static void processInput(GLFWwindow *window) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
    }
};
