#include "sb7.h"
#include <cmath>
#include <cstring>
#include <stddef.h>

struct vertex {
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat r;
    GLfloat g;
    GLfloat b;
};

class my_appliction : public sb7::application {
public:
    void startup() override {
        program = compile_shaders();
        glUseProgram(program);

        glCreateVertexArrays(1, &vao);

        static const vertex vertices[] {
            { 0.25, -0.25, 0.5, 1.0, 0.0, 0.0 },  //
            { -0.25, -0.25, 0.5, 0.0, 1.0, 0.0 }, //
            { 0.25, 0.25, 0.5, 0.0, 0.0, 1.0 },   //
        };

        GLuint buffer;
        glCreateBuffers(1, &buffer);

        glNamedBufferStorage(buffer, sizeof(vertices), vertices,
                             GL_MAP_WRITE_BIT);

        glVertexArrayVertexBuffer(vao, 0, buffer, 0, sizeof(vertex));

        glEnableVertexArrayAttrib(vao, 0);
        glVertexArrayAttribBinding(vao, 0, 0);
        glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE,
                                  offsetof(vertex, x));

        glEnableVertexArrayAttrib(vao, 1);
        glVertexArrayAttribBinding(vao, 1, 0);
        glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE,
                                  offsetof(vertex, r));

        glBindVertexArray(vao);
    }

    void render(double currentTime) override {
        const GLfloat color[] { 0.7137f, 0.7333f, 0.7686f, 1.0f };
        glClearBufferfv(GL_COLOR, 0, color);

        glUseProgram(program);

        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void shutdown() override {
        glDeleteVertexArrays(1, &program);
        glDeleteProgram(program);
    }

private:
    GLuint program;
    GLuint vao;

private:
    GLuint compile_shaders() {
        GLuint vertex_shader;
        GLuint fragment_shader;
        GLuint program;

        // Source code for vertex shader
        static const GLchar *vertex_shader_source[] { R"(
            #version 450 core

            layout (location = 0) in vec3 position;
            layout (location = 1) in vec3 color;

            out vec3 vs_color;

            void main() {
                gl_Position = vec4(position, 1.0);
                vs_color = color;
            }
        )" };

        // Source code for fragment shader
        static const GLchar *fragment_shader_source[] {
            R"(
                #version 450 core

                in vec3 vs_color;

                out vec4 color;

                void main() {
                    color = vec4(vs_color, 1.0);
                }
            )"
        };

        // Create and compile vertex shader
        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, vertex_shader_source, nullptr);
        glCompileShader(vertex_shader);

        // Create and compile fragment shader
        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, fragment_shader_source, nullptr);
        glCompileShader(fragment_shader);

        // Create program, attach shaders to it, and link it
        program = glCreateProgram();
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);
        glLinkProgram(program);

        // Delete the shaders as the program has them now
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        return program;
    }
};

DECLARE_MAIN(my_appliction);
