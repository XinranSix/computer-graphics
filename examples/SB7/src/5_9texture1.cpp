#include "sb7.h"
#include "vmath.h"
#include <iostream>

class my_appliction : public sb7::application {
public:
    void startup() override {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexStorage2D(GL_TEXTURE_2D, 8, GL_RGBA32F, 256, 256);

        float *data { new float[256 * 256 * 4] };
        generate_texture(data, 256, 256);

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 256, GL_RGBA, GL_FLOAT,
                        data);

        delete[] data;

        program = compile_shaders();
        glUseProgram(program);

        glCreateVertexArrays(1, &vao);
        glBindVertexArray(vao);
    }

    void render(double currentTime) override {
        static const GLfloat green[] = { 0.0f, 0.25f, 0.0f, 1.0f };
        glClearBufferfv(GL_COLOR, 0, green);

        glUseProgram(program);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void shutdown() override {
        glDeleteVertexArrays(1, &program);
        glDeleteProgram(program);
    }

private:
    GLuint compile_shaders() {
        GLuint vertex_shader;
        GLuint fragment_shader;
        GLuint program;

        // Source code for vertex shader
        static const GLchar *vertex_shader_source[] { R"(
            #version 450 core

            void main() {
                const vec4 vertices[] = vec4[](
                    vec4(0.75, -0.75, 0.5, 1.0),
                    vec4(-0.75, -0.75, 0.5, 1.0),
                    vec4(0.75, 0.75, 0.5, 1.0)
                );

                gl_Position = vertices[gl_VertexID];
            }
        )" };

        // Source code for fragment shader
        static const GLchar *fragment_shader_source[] {
            R"(
                #version 450 core

                uniform sampler2D s;

                out vec4 color;

                void main() {
                    color = texture(s, gl_FragCoord.xy / textureSize(s, 0));   
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

private:
    void generate_texture(float *data, int width, int height) {
        for (int y {}; y < height; ++y) {
            for (int x {}; x < width; ++x) {
                data[(y * width + x) * 4 + 0] =
                    (float)((x & y) & 0xFF) / 255.0f;
                data[(y * width + x) * 4 + 1] =
                    (float)((x | y) & 0xFF) / 255.0f;
                data[(y * width + x) * 4 + 2] =
                    (float)((x ^ y) & 0xFF) / 255.0f;
                data[(y * width + x) * 4 + 3] = 1.0f;
            }
        }
    }

private:
    GLuint texture;
    GLuint program;
    GLuint vao;
};

DECLARE_MAIN(my_appliction);
