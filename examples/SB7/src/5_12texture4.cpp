#include <algorithm>
#include <iterator>
#include <sb7.h>
#include <vmath.h>

#include <object.h>
#include <shader.h>
#include <sb7ktx.h>
#include <iostream>

class my_appliction : public sb7::application {
public:
protected:
    void init() override {
        static const char title[] = "OpenGL SuperBible - Tunnel";

        sb7::application::init();

        memcpy(info.title, title, sizeof(title));
    }

    void startup() override {

        loadShaders();

        glCreateVertexArrays(1, &render_vao);
        glBindVertexArray(render_prog);

        tex_wall = sb7::ktx::file::load("asset/sb7/textures/brick.ktx");
        tex_ceiling = sb7::ktx::file::load("asset/sb7/textures/ceiling.ktx");
        tex_floor = sb7::ktx::file::load("asset/sb7/textures/floor.ktx");

        GLuint textures[] { tex_wall, tex_ceiling, tex_floor };

        for (int i {}; i < 3; i++) {
            glBindTexture(GL_TEXTURE_2D, textures[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                            GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }

        glBindVertexArray(render_vao);
    }

    void render(double currentTime) override {
        static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        float t = (float)currentTime;

        glViewport(0, 0, info.windowWidth, info.windowHeight);
        glClearBufferfv(GL_COLOR, 0, black);

        glUseProgram(render_prog);

        vmath::mat4 proj_matrix = vmath::perspective(
            60.0f, (float)info.windowWidth / (float)info.windowHeight, 0.1f,
            100.0f);

        glUniform1f(uniforms.offset, t * 0.003f);

        GLuint textures[] = { tex_wall, tex_floor, tex_wall, tex_ceiling };
        for (int i {}; i < 4; i++) {
            vmath::mat4 mv_matrix =
                vmath::rotate(90.0f * (float)i, vmath::vec3(0.0f, 0.0f, 1.0f)) *
                vmath::translate(-0.5f, 0.0f, -10.0f) *
                vmath::rotate(90.0f, 0.0f, 1.0f, 0.0f) *
                vmath::scale(30.0f, 1.0f, 1.0f);
            vmath::mat4 mvp = proj_matrix * mv_matrix;

            glUniformMatrix4fv(uniforms.mvp, 1, GL_FALSE, mvp);

            glBindTexture(GL_TEXTURE_2D, textures[i]);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
    }

private:
    void loadShaders() {
        // Source code for vertex shader
        static const GLchar *vertex_shader_source[] {
            R"(
                #version 450 core

                out VS_OUT {
                    vec2 tc;
                } vs_out;

                uniform mat4 mvp;
                uniform float offset;

                void main() {
                    const vec2[4] position = vec2[4](
                        vec2(-0.5, -0.5),
                        vec2( 0.5, -0.5),
                        vec2(-0.5,  0.5),
                        vec2( 0.5,  0.5)
                    );
                    vs_out.tc = (position[gl_VertexID].xy + vec2(offset, 0.5)) * vec2(30.0, 1.0);
                    gl_Position = mvp * vec4(position[gl_VertexID], 0.0, 1.0);
                }
            )"
        };

        // Source code for fragment shader
        static const GLchar *fragment_shader_source[] {
            R"(
                #version 450 core

                layout (location = 0) out vec4 color;

                in VS_OUT {
                    vec2 tc;
                } fs_in;

                layout (binding = 0) uniform sampler2D tex;

                void main() {
                    color = texture(tex, fs_in.tc);
                }
            )"
        };

        if (render_prog)
            glDeleteProgram(render_prog);

        GLuint vertex_shader;
        GLuint fragment_shader;

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
        render_prog = glCreateProgram();
        glAttachShader(render_prog, vertex_shader);
        glAttachShader(render_prog, fragment_shader);
        glLinkProgram(render_prog);
        glGetProgramiv(render_prog, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(render_prog, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                      << infoLog << std::endl;
        }

        // Delete the shaders as the program has them now
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        uniforms.mvp = glGetUniformLocation(render_prog, "mvp");
        uniforms.offset = glGetUniformLocation(render_prog, "offset");
    }

private:
protected:
    GLuint render_prog;
    GLuint render_vao;
    struct {
        GLint mvp;
        GLint offset;
    } uniforms;

    GLuint tex_wall;
    GLuint tex_ceiling;
    GLuint tex_floor;
};

DECLARE_MAIN(my_appliction);
