#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

static const GLchar *vertex_shader_source[] {
    R"(
        #version 450 core

        layout (location = 0) in vec4 position;
        layout (location = 1) in vec4 color;

        out vec4 vs_color;

        void main() {
            gl_Position = position;
            vs_color = color;
        }
    )"
};

static const GLchar *fragment_shader_source[] {
    R"(
        #version 450 core

        in vec4 vs_color;

        out vec4 color;

        void main() {
            color = vs_color;
        }
    )"
};

static const GLfloat data[] {
    0.25,  -0.25, 0.5, 1.0, 1.0, 0.0, 0.0, 1.0, //
    -0.25, -0.25, 0.5, 1.0, 0.0, 1.0, 0.0, 1.0, //
    0.25,  0.25,  0.5, 1.0, 0.0, 0.0, 1.0, 1.0  //
};

static const GLfloat color[] { 0.7137f, 0.7333f, 0.7686f, 1.0f };

int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window { glfwCreateWindow(800, 600, "OpenGL", nullptr,
                                          nullptr) };
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

    GLuint vertex_shader, fragment_shader, program;

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, vertex_shader_source, nullptr);
    glCompileShader(vertex_shader);
    int success;
    char infoLog[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    GLuint vao, vbo;

    glCreateVertexArrays(1, &vao);

    glCreateBuffers(1, &vbo);

    glNamedBufferStorage(vao, sizeof(data), data, GL_MAP_WRITE_BIT);
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(GLfloat) * 8);

    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayAttribFormat(vao, 0, 4, GL_FLOAT, GL_FALSE, 0);
    glEnableVertexArrayAttrib(vao, 0);

    glVertexArrayAttribBinding(vao, 1, 0);
    glVertexArrayAttribFormat(vao, 1, 4, GL_FLOAT, GL_FALSE,
                              sizeof(GLfloat) * 4);
    glEnableVertexArrayAttrib(vao, 1);

    while (!glfwWindowShouldClose(window)) {
        glClearBufferfv(GL_COLOR, 0, color);

        glBindVertexArray(vao);
        glUseProgram(program);

        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteProgram(program);
    glfwTerminate();

    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}
