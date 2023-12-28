#include <algorithm>
#include <format>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <iterator>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>
#include <source_location>
#include <vector>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

class MyException : public std::exception {
public:
    MyException(std::string message,
                std::source_location location = std::source_location::current())
        : message { std::move(message) }, location { std::move(location) } {
    }

    const char *what() const noexcept override {
        return message.c_str();
    }

    virtual const std::source_location where() const noexcept {
        return location;
    }

private:
    std::string message;
    std::source_location location;
};

struct WindownInfo {
public:
    int width { SCR_WIDTH };
    int height { SCR_HEIGHT };
    std::string title { TITLE };

private:
    // settings
    constexpr static unsigned int SCR_WIDTH { 800 };
    constexpr static unsigned int SCR_HEIGHT { 600 };
    constexpr static const char *TITLE { "LearnOpenGL" };
};

GLFWwindow *window {};

void shutdown() {
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
}

WindownInfo widownInfo {};

std::vector<std::pair<int, int>> windowHintInfos {
    { GLFW_CONTEXT_VERSION_MAJOR, 3 },
    { GLFW_CONTEXT_VERSION_MINOR, 3 },
    { GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE }
};

void init() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    std::for_each(std::cbegin(windowHintInfos), std::cend(windowHintInfos),
                  [](auto &windowHintInfo) {
                      glfwWindowHint(windowHintInfo.first,
                                     windowHintInfo.second);
                  });

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(widownInfo.width, widownInfo.height,
                              widownInfo.title.c_str(), nullptr, nullptr);

    if (window == nullptr) {
        shutdown();
        throw MyException { "Failed to create GLFW window" };
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        shutdown();
        throw MyException { "Failed to initialize GLAD" };
    }
}

void run() {
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // glfw: swap buffers and poll IO events (keys pressed/released,
        // mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

int main() {

    try {
        init();
        run();
    } catch (const MyException &e) {
        const auto &location { e.where() };
        std::cerr << std::format("Caught: '{}' at line {} in {}.", e.what(),
                                 location.line(), location.function_name())
                  << std::endl;
    }
    shutdown();

    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released
// this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this
// callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that
    // width and height will be significantly larger than specified on
    // retina displays.
    glViewport(0, 0, width, height);
}
