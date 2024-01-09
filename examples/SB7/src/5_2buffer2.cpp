#include "sb7.h"
#include <cmath>
#include <cstring>

class my_appliction : public sb7::application {
public:
    void startup() override {
        rendering_program = compile_shaders();
        glCreateVertexArrays(1, &vertex_array_object);
        glBindVertexArray(vertex_array_object);

        // The type used for names in OpenGL is GLuint
        GLuint buffer;

        // Create a buffer
        glCreateBuffers(1, &buffer);

        // Specify the data store parameters for the buffer
        glNamedBufferStorage(buffer,      // Name for the buffer
                             1024 * 1024, // 1 MiB of space
                             nullptr,     // No initial data
                             GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT |
                                 GL_MAP_PERSISTENT_BIT);

        // Now bind it to the context using the GL_ARRAY_BUFFER binding point
        glBindBuffer(GL_ARRAY_BUFFER, buffer);

        // This is the data that we will place into the buffer object
        static const float data[] {
            0.25,  -0.25, 0.5, 1.0, //
            -0.25, -0.25, 0.5, 1.0, //
            0.25,  0.25,  0.5, 1.0  //
        };

        // Get a pointer to the buffer's data store
        void *ptr { glMapNamedBuffer(buffer, GL_WRITE_ONLY) };

        // Copy ouer data into it...
        memcpy(ptr, data, sizeof(data));

        //  Tell OpenGL that we're done with the pointer
        glUnmapNamedBuffer(buffer);
    }

    void render(double currentTime) override {
        const GLfloat color[] {
            static_cast<float>(std::sin(currentTime) * 0.5f + 0.5f),
            static_cast<float>(std::cos(currentTime) * 0.5f + 0.5f), 0.0f, 1.0f
        };
        glClearBufferfv(GL_COLOR, 0, color);

        glUseProgram(rendering_program);

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

            out vec4 vs_color;

            void main() {
                const vec4 vertices[3] = vec4[3](
                    vec4( 0.25, -0.25, 0.5, 1.0),
                    vec4(-0.25, -0.25, 0.5, 1.0),
                    vec4( 0.25,  0.25, 0.5, 1.0)
                );

                const vec4 colors[3] = vec4[3](
                    vec4(1.0, 0.0, 0.0, 1.0),
                    vec4(0.0, 1.0, 0.0, 1.0),
                    vec4(0.0, 0.0, 1.0, 1.0)
                );

                gl_Position = vertices[gl_VertexID];

                vs_color = colors[gl_VertexID];
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
