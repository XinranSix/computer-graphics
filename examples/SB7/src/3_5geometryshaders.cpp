#include "sb7.h"

class my_appliction : public sb7::application {
public:
    void startup() override {
        rendering_program = compile_shaders();
        glCreateVertexArrays(1, &vertex_array_object);
        glBindVertexArray(vertex_array_object);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    void render(double currentTime) override {
        static const GLfloat green[] = { 0.0f, 0.25f, 0.0f, 1.0f };
        glClearBufferfv(GL_COLOR, 0, green);

        glUseProgram(rendering_program);
        glPointSize(5.0f);
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
        GLuint gs;
        GLuint fs;
        GLuint program;

        // Source code for vertex shader
        static const GLchar *vs_source[] { R"(
            #version 450 core

            void main() {
                const vec4 vertices[3] = vec4[3](
                    vec4(0.25,-0.25,0.5,1.0),
                    vec4(-0.25,-0.25,0.5,1.0),
                    vec4(0.25,0.25,0.5,1.0)
                );

                // Add 'offset' to our hard-coded vertex position
                gl_Position = vertices[gl_VertexID];
            }
        )" };

        // Source code for tessellation control shader
        static const GLchar *tcs_source[] { R"(
            #version 450 core

            layout (vertices = 3) out;

            void main() {
                // Only if I am invocation 0 ...
                if (gl_InvocationID == 0) {
                    gl_TessLevelInner[0] = 4.0;
                    gl_TessLevelOuter[0] = 4.0;
                    gl_TessLevelOuter[1] = 4.0;
                    gl_TessLevelOuter[2] = 4.0;
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

        // Source code for geometry shader
        static const GLchar *gs_source[] { R"(
            #version 450 core

            layout (triangles) in;
            layout (points, max_vertices = 3) out;

            void main() {
                int i;

                for (i = 0; i < gl_in.length(); i++) {
                    gl_Position = gl_in[i].gl_Position;
                    EmitVertex();
                }
            }
        )" };

        // Source code for fragment shader
        static const GLchar *fs_source[] {
            R"(
                #version 450 core

                // Output to the framebuffer
                out vec4 color;

                void main() {
                    // Simply assign the color we were given by the vertex shader to our output
                    color = vec4(0.0, 0.8, 1.0, 1.0);     
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

        // Create and compile geometry shader
        gs = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(gs, 1, gs_source, nullptr);
        glCompileShader(gs);

        // Create and compile fragment shader
        fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, fs_source, nullptr);
        glCompileShader(fs);

        // Create program, attach shaders to it, and link it
        program = glCreateProgram();
        glAttachShader(program, vs);
        glAttachShader(program, tcs);
        glAttachShader(program, tes);
        glAttachShader(program, gs);
        glAttachShader(program, fs);
        glLinkProgram(program);

        // Delete the shaders as the program has them now
        glDeleteShader(vs);
        glDeleteShader(tcs);
        glDeleteShader(tes);
        glDeleteShader(gs);
        glDeleteShader(fs);

        return program;
    }
};

DECLARE_MAIN(my_appliction);
