#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/gtx/transform.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
static const GLfloat vertex_position[] {
    -0.25f, -0.25f, 0.25f,  //
    -0.25f, -0.25f, -0.25f, //
    0.25f,  -0.25f, -0.25f, //

    0.25f,  -0.25f, -0.25f, //
    0.25f,  -0.25f, 0.25f,  //
    -0.25f, -0.25f, 0.25f,  //

    0.25f,  -0.25f, -0.25f, //
    0.25f,  0.25f,  -0.25f, //
    0.25f,  -0.25f, 0.25f,  //

    0.25f,  0.25f,  -0.25f, //
    0.25f,  0.25f,  0.25f,  //
    0.25f,  -0.25f, 0.25f,  //

    0.25f,  0.25f,  -0.25f, //
    -0.25f, 0.25f,  -0.25f, //
    0.25f,  0.25f,  0.25f,  //

    -0.25f, 0.25f,  -0.25f, //
    -0.25f, 0.25f,  0.25f,  //
    0.25f,  0.25f,  0.25f,  //

    -0.25f, 0.25f,  -0.25f, //
    -0.25f, -0.25f, -0.25f, //
    -0.25f, 0.25f,  0.25f,  //

    -0.25f, -0.25f, -0.25f, //
    -0.25f, -0.25f, 0.25f,  //
    -0.25f, 0.25f,  0.25f,  //

    -0.25f, 0.25f,  -0.25f, //
    0.25f,  0.25f,  -0.25f, //
    0.25f,  -0.25f, -0.25f, //

    0.25f,  -0.25f, -0.25f, //
    -0.25f, -0.25f, -0.25f, //
    -0.25f, 0.25f,  -0.25f, //

    -0.25f, -0.25f, 0.25f, //
    0.25f,  -0.25f, 0.25f, //
    0.25f,  0.25f,  0.25f, //

    0.25f,  0.25f,  0.25f, //
    -0.25f, 0.25f,  0.25f, //
    -0.25f, -0.25f, 0.25f  //
};

static const GLchar *vertex_shader_source[] {
    R"(
            #version 450 core

            layout (location = 0) in vec4 position;

            out VS_OUT {
                vec4 color;
            } vs_out;

            uniform mat4 mv_matrix;
            uniform mat4 proj_matrix;

            void main() {
                gl_Position = proj_matrix * mv_matrix * position;
                vs_out.color = position * 2.0 + vec4(0.5, 0.5, 0.5, 0.0);
            }
        )"
};

static const GLchar *fragment_shader_source[] {
    R"(
                #version 450 core

                out vec4 color;

                in VS_OUT {
                    vec4 color;
                } fs_in;

                void main() {
                    color = fs_in.color;
                }
            )"
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
    glBindVertexArray(vao);
    glCreateBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_position), vertex_position,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3,
                          nullptr);
    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(window)) {
        glClearBufferfv(GL_COLOR, 0, color);

        auto currentTime { static_cast<float>(glfwGetTime()) };
        float f { static_cast<float>(currentTime * glm::pi<float>() * 0.1) };
        auto proj_matrix { glm::perspective(50.0f, (float)800 / (float)600,
                                            0.1f, 1000.0f) };
        auto mv_matrix {

            glm::translate(glm::vec3 { 0.0f, 0.0f, -4.0f }) *
            glm::translate(glm::vec3 {
                glm::sin(2.1f * f) * 0.5f, glm::cos(1.7f * f) * 0.5f,
                glm::sin(1.3f * f) * glm::cos(1.5f * f) * 2.0f }) *
            glm::rotate(static_cast<float>(currentTime) * 45.0f,
                        glm::vec3 { 0.0f, 1.0f, 0.0f }) *
            glm::rotate(static_cast<float>(currentTime) * 81.0f,
                        glm::vec3 { 1.0f, 0.0f, 0.0f })
        };

        glBindVertexArray(vao);
        glUseProgram(program);

        glUniformMatrix4fv(glGetUniformLocation(program, "mv_matrix"), 1,
                           GL_FALSE, glm::value_ptr(mv_matrix));
        glUniformMatrix4fv(glGetUniformLocation(program, "proj_matrix"), 1,
                           GL_FALSE, glm::value_ptr(proj_matrix));

        glDrawArrays(GL_TRIANGLES, 0, 36);
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
