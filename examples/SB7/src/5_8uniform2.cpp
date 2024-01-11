#include "sb7.h"
#include "vmath.h"
#include <cmath>
#include <iostream>

class my_appliction : public sb7::application {
public:
    void startup() override {
        program = compile_shaders();
        glUseProgram(program);

        glCreateVertexArrays(1, &vao);
        glBindVertexArray(vao);

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

        GLuint buffer;
        glCreateBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_position), vertex_position,
                     GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                              nullptr);
        glEnableVertexAttribArray(0);
    }

    void render(double currentTime) override {

        vmath::mat4 proj_matrix = vmath::perspective(
            45.0f, (float)info.windowWidth / (float)info.windowHeight, 0.1f,
            50.0f);

        const GLfloat color[] { 0.7137f, 0.7333f, 0.7686f, 1.0f };
        glClearBufferfv(GL_COLOR, 0, color);

        glUseProgram(program);

        glUniformMatrix4fv(glGetUniformLocation(program, "proj_matrix"), 1,
                           GL_FALSE, proj_matrix);

        glViewport(0, 0, info.windowWidth, info.windowHeight);
    }

    void shutdown() override {
        glDeleteVertexArrays(1, &program);
        glDeleteProgram(program);
    }

private:
    GLuint program;
    GLuint vao;
    float aspect;

private:
    GLuint compile_shaders() {
        GLuint vertex_shader;
        GLuint fragment_shader;
        GLuint program;

        // Source code for vertex shader
        static const GLchar *vertex_shader_source[] { R"(
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
        )" };

        // Source code for fragment shader
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

        // Create and compile vertex shader
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

        // Create and compile fragment shader
        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, fragment_shader_source, nullptr);
        glCompileShader(fragment_shader);
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragment_shader, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                      << infoLog << std::endl;
        }

        // Create program, attach shaders to it, and link it
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

        // Delete the shaders as the program has them now
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        return program;
    }
};

DECLARE_MAIN(my_appliction);
