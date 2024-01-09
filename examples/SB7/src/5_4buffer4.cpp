#include "sb7.h"
#include <cmath>
#include <cstring>

class my_appliction : public sb7::application {
public:
    void startup() override {
        program = compile_shaders();
        glUseProgram(program);

        glCreateVertexArrays(1, &vao);
        // glBindVertexArray(vao);

        static const GLfloat data[] {
            0.25,  -0.25, 0.5, 1.0, 1.0, 0.0, 0.0, 1.0, //
            -0.25, -0.25, 0.5, 1.0, 0.0, 1.0, 0.0, 1.0, //
            0.25,  0.25,  0.5, 1.0, 0.0, 0.0, 1.0, 1.0  //
        };

        GLuint buffer;
        glCreateBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);

        glNamedBufferStorage(buffer, sizeof(data), data, GL_MAP_WRITE_BIT);

        glVertexArrayVertexBuffer(vao, 0, buffer, 0, sizeof(GLfloat) * 8);

        glEnableVertexArrayAttrib(vao, 0);
        glVertexArrayAttribBinding(vao, 0, 0);
        glVertexArrayAttribFormat(vao, 0, 4, GL_FLOAT, GL_FALSE, 0);

        glEnableVertexArrayAttrib(vao, 1);
        glVertexArrayAttribBinding(vao, 1, 0);
        glVertexArrayAttribFormat(vao, 1, 4, GL_FLOAT, GL_FALSE,
                                  sizeof(GLfloat) * 4);

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

            layout (location = 0) in vec4 position;
            layout (location = 1) in vec4 color;

            out vec4 vs_color;

            void main() {
                gl_Position = position;
                vs_color = color;
            }
        )" };

        // Source code for fragment shader
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
