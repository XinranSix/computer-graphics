#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/gtx/transform.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "shader/shader.h"

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

    ht::opengl::ShaderProgram program {};

    if (!program.addShaderFromSourceFile(ht::opengl::ShaderType::VERTEX,
                                         "asset/shader/test/test.vert")) {
        std::cout << program.log() << std::endl;
    }
    if (!program.addShaderFromSourceFile(ht::opengl::ShaderType::FRAGMENT,
                                         "asset/shader/test/test.frag")) {
        std::cout << program.log() << std::endl;
    }

    program.create();
    if (!program.link()) {
        std::cout << program.log() << std::endl;
    }

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
        auto proj_matrix { glm::perspective(50.0f, (float)800 / (float)600,
                                            0.1f, 1000.0f) };

        glBindVertexArray(vao);
        program.bind();

        glUniformMatrix4fv(
            glGetUniformLocation(program.programId(), "proj_matrix"), 1,
            GL_FALSE, glm::value_ptr(proj_matrix));

        glDrawArrays(GL_TRIANGLES, 0, 36);
        for (int i {}; i < 24; ++i) {
            auto f { static_cast<float>(i + currentTime * 0.3f) };
            glm::mat4 mv_matrix {
                glm::translate(glm::vec3 { 0.0f, 0.0f, -20.0f }) *
                glm::rotate(static_cast<float>(currentTime * 45.0f),
                            glm::vec3 { 0.0f, 1.0f, 0.0f }) *
                glm::rotate(static_cast<float>(currentTime * 21.0f),
                            glm::vec3 { 1.0f, 0.0f, 0.0f }) *
                glm::translate(
                    glm::vec3 { sinf(2.1f * f) * 2.0f, cosf(1.7f * f) * 2.0f,
                                sinf(1.3f * f) * cosf(1.5f * f) * 2.0f })
            };

            glUniformMatrix4fv(
                glGetUniformLocation(program.programId(), "mv_matrix"), 1,
                GL_FALSE, glm::value_ptr(mv_matrix));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
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
