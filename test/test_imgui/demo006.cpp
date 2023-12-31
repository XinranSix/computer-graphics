#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class GLFWWindow {
public:
    GLFWWindow(int width, int height, const char *title)
        : width(width), height(height), title(title), window(nullptr) {
        if (!Init()) {
            std::cerr << "Failed to initialize GLFW\n";
            std::exit(EXIT_FAILURE);
        }

        CreateWindow();
        SetupCallbacks();
    }

    ~GLFWWindow() {
        glfwTerminate();
    }

    void MainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            // 在这里添加你的OpenGL渲染代码

            glfwSwapBuffers(window);
        }
    }

private:
    int width, height;
    const char *title;
    GLFWwindow *window;

    bool Init() {
        if (!glfwInit()) {
            return false;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        return true;
    }

    void CreateWindow() {
        window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!window) {
            std::cerr << "Failed to create GLFW window\n";
            glfwTerminate();
            std::exit(EXIT_FAILURE);
        }

        glfwMakeContextCurrent(window);

        // 使用 glad 加载 OpenGL 函数指针
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD\n";
            glfwTerminate();
            std::exit(EXIT_FAILURE);
        }
    }

    static void ErrorCallback(int error, const char *description) {
        std::cerr << "GLFW Error " << error << ": " << description << std::endl;
    }

    static void KeyCallback(GLFWwindow *window, int key, int scancode,
                            int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }

    void SetupCallbacks() {
        glfwSetErrorCallback(ErrorCallback);
        glfwSetKeyCallback(window, KeyCallback);
    }
};

int main() {
    GLFWWindow glfwWindow(800, 600, "GLFW Window");
    glfwWindow.MainLoop();

    return 0;
}
