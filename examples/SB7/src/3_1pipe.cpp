#include "sb7.h"
#include <cmath>

class my_appliction : public sb7::application {
public:
    void startup() override {
        rendering_program = compile_shaders();
        glCreateVertexArrays(1, &vertex_array_object);
        glBindVertexArray(vertex_array_object);
    }

    void render(double currentTime) override {
        const GLfloat color[] {
            static_cast<float>(std::sin(currentTime) * 0.5f + 0.5f),
            static_cast<float>(std::cos(currentTime) * 0.5f + 0.5f), 0.0f, 1.0f
        };
        glClearBufferfv(GL_COLOR, 0, color);

        glUseProgram(rendering_program);

        GLfloat attrib[] { static_cast<float>(std::sin(currentTime)) * 0.5f,
                           static_cast<float>(std::cos(currentTime)) * 0.6f,
                           0.0f, 0.0f };

        // Update the value of input attribute 0
        glVertexAttrib4fv(0, attrib);
        const GLfloat color1[] { 0, 0, 0, 0 };
        glVertexAttrib4fv(1, color1);

        // Draw on triangle
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void shutdown() override {
        glDeleteVertexArrays(1, &vertex_array_object);
        glDeleteProgram(rendering_program);
    }

private:
    GLuint rendering_program;
    GLuint vertex_array_object;

private:
    GLuint compile_shaders() {
        GLuint vertex_shader;
        GLuint fragment_shader;
        GLuint program;

        // Source code for vertex shader
        static const GLchar *vertex_shader_source[] { R"(
            #version 450 core

            // 'offset' is an input vertex attribute
            layout (location = 0) in vec4 offset;
            layout (location = 1) in vec4 color;

            // 'vs_color' is an output that will be sent to the next shader stage
            out vec4 vs_color;

            void main() {
                const vec4 vertices[3] = vec4[3](
                    vec4(0.25,-0.25,0.5,1.0),
                    vec4(-0.25,-0.25,0.5,1.0),
                    vec4(0.25,0.25,0.5,1.0)
                );

                gl_Position = vertices[gl_VertexID] + offset;

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
