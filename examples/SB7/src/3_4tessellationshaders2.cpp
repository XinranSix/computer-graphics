#include "sb7.h"
#include <cmath>

class my_appliction : public sb7::application {
public:
    void startup() override {
        rendering_program = compile_shaders();
        glCreateVertexArrays(1, &vertex_array_object);
        glBindVertexArray(vertex_array_object);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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

        glDrawArrays(GL_PATCHES, 0, 3);
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
        GLuint vs;
        GLuint tcs;
        GLuint tes;
        GLuint fs;
        GLuint program;

        // Source code for vertex shader
        static const GLchar *vs_source[] { R"(
            #version 450 core

            // 'offset' is an input vertex attribute
            layout (location = 0) in vec4 offset;
            layout (location = 1) in vec4 color;

            // Declare VS_OUT as an output interface block
            out VS_OUT {
                vec4 color; // Send color to the next stage
            } vs_out;

            void main() {
                const vec4 vertices[3] = vec4[3](
                    vec4(0.25,-0.25,0.5,1.0),
                    vec4(-0.25,-0.25,0.5,1.0),
                    vec4(0.25,0.25,0.5,1.0)
                );

                // Add 'offset' to our hard-coded vertex position
                gl_Position = vertices[gl_VertexID] + offset;

                vs_out.color = color;
            }
        )" };

        // Source code for tessellation control shader
        static const GLchar *tcs_source[] { R"(
            #version 450 core

            layout (vertices = 3) out;

            void main() {
                // Only if I am invocation 0 ...
                if (gl_InvocationID == 0) {
                    gl_TessLevelInner[0] = 3.0;
                    gl_TessLevelOuter[0] = 3.0;
                    gl_TessLevelOuter[1] = 3.0;
                    gl_TessLevelOuter[2] = 3.0;
                }
                // Everybody copies their input to their output
                gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
            }
        )" };

        static const GLchar *tes_source[] { R"(
            #version 450 core

            layout (triangles, equal_spacing, cw) in;

            void main() {
                gl_Position = 
                (gl_TessCoord.x * gl_in[0].gl_Position) + 
                (gl_TessCoord.y * gl_in[1].gl_Position) + 
                (gl_TessCoord.z * gl_in[2].gl_Position);
            }
        )" };

        // Source code for fragment shader
        static const GLchar *fs_source[] {
            R"(
                #version 450 core

                // Declare VS_OUT as an input interface block
                in VS_OUT {
                    vec4 color; // Send color to the next stage
                } fs_in;

                // Output to the framebuffer
                out vec4 color;

                void main() {
                    // Simply assign the color we were given by the vertex shader to our output
                    color = fs_in.color;
                }
            )"
        };

        // Create and compile vertex shader
        vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, vs_source, nullptr);
        glCompileShader(vs);

        // Create and compile tessellation control shader
        tcs = glCreateShader(GL_TESS_CONTROL_SHADER);
        glShaderSource(tcs, 1, tcs_source, nullptr);
        glCompileShader(tcs);

        // Create and compile tessellation evaluation shader
        tes = glCreateShader(GL_TESS_EVALUATION_SHADER);
        glShaderSource(tes, 1, tes_source, nullptr);
        glCompileShader(tes);

        // Create and compile fragment shader
        fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, fs_source, nullptr);
        glCompileShader(fs);

        // Create program, attach shaders to it, and link it
        program = glCreateProgram();
        glAttachShader(program, vs);
        glAttachShader(program, tcs);
        glAttachShader(program, tes);
        glAttachShader(program, fs);
        glLinkProgram(program);

        // Delete the shaders as the program has them now
        glDeleteShader(vs);
        glDeleteShader(tcs);
        glDeleteShader(tes);
        glDeleteShader(fs);

        return program;
    }
};

DECLARE_MAIN(my_appliction);
