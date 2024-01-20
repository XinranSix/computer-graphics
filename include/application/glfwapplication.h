#pragma once

#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include <string>

class GLFWApplication {
public:
    GLFWApplication() = default;
    virtual ~GLFWApplication() = default;

    virtual void init() = 0;
    virtual void render(double currentTime) = 0;
    virtual void shutdown() = 0;


protected:
    GLFWwindow *window;
    int width;
    int height;
    std::string windowTitle;
};
