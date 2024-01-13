#include <sb7.h>
#include <vmath.h>

#include <object.h>
#include <shader.h>
#include <sb7ktx.h>
#include <iostream>

class my_appliction : public sb7::application {
public:
    my_appliction() : render_prog { 0 }, tex_index { 0 } {
    }

protected:
    void init() override {
        static const char title[] = "OpenGL SuperBible - Texture Coordinates";

        sb7::application::init();

        memcpy(info.title, title, sizeof(title));
    }

    void startup() override {
#define B 0x00, 0x00, 0x00, 0x00
#define W 0xFF, 0xFF, 0xFF, 0xFF
        static const GLubyte tex_data[] = {
            B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, W, B, W, B, W, B,
            W, B, W, B, W, B, W, B, W, B, B, W, B, W, B, W, B, W, B, W, B, W,
            B, W, B, W, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, B, W,
            B, W, B, W, B, W, B, W, B, W, B, W, B, W, W, B, W, B, W, B, W, B,
            W, B, W, B, W, B, W, B, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
            B, W, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, B, W, B, W,
            B, W, B, W, B, W, B, W, B, W, B, W, W, B, W, B, W, B, W, B, W, B,
            W, B, W, B, W, B, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W,
            W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, B, W, B, W, B, W,
            B, W, B, W, B, W, B, W, B, W, W, B, W, B, W, B, W, B, W, B, W, B,
            W, B, W, B, B, W, B, W, B, W, B, W, B, W, B, W, B, W, B, W, W, B,
            W, B, W, B, W, B, W, B, W, B, W, B, W, B,
        };
#undef B
#undef W

        glGenTextures(1, &tex_object[0]);
        glBindTexture(GL_TEXTURE_2D, tex_object[0]);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 16, 16);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 16, 16, GL_RGBA,
                        GL_UNSIGNED_BYTE, tex_data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        tex_object[1] = sb7::ktx::file::load("asset/sb7/textures/pattern1.ktx");
        object.load("asset/sb7/objects/torus_nrms_tc.sbm");

        loadShaders();

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }

    void render(double currentTime) override {
        static const GLfloat gray[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        static const GLfloat ones[] = { 1.0f };

        glClearBufferfv(GL_COLOR, 0, gray);
        glClearBufferfv(GL_DEPTH, 0, ones);

        glViewport(0, 0, info.windowWidth, info.windowHeight);

        glBindTexture(GL_TEXTURE_2D, tex_object[tex_index]);

        glUseProgram(render_prog);

        vmath::mat4 proj_matrix = vmath::perspective(
            60.0f, (float)info.windowWidth / (float)info.windowHeight, 0.1f,
            1000.0f);
        vmath::mat4 mv_matrix =
            vmath::translate(0.0f, 0.0f, -3.0f) *
            vmath::rotate((float)currentTime * 19.3f, 0.0f, 1.0f, 0.0f) *
            vmath::rotate((float)currentTime * 21.1f, 0.0f, 0.0f, 1.0f);

        glUniformMatrix4fv(uniforms.mv_matrix, 1, GL_FALSE, mv_matrix);
        glUniformMatrix4fv(uniforms.proj_matrix, 1, GL_FALSE, proj_matrix);

        object.render();
    }

    void shutdown() override {
        glDeleteProgram(render_prog);
        glDeleteTextures(2, tex_object);
    }

private:
    void loadShaders() {
        // Source code for vertex shader
        static const GLchar *vertex_shader_source[] { R"(
               #version 450 core

               uniform mat4 mv_matrix;
               uniform mat4 proj_matrix;

               layout (location = 0) in vec4 position;
               layout (location = 4) in vec2 tc;

               out VS_OUT {
                   vec2 tc;
               } vs_out;

               void main() {
                   vec4 pos_vs = mv_matrix * position;
                   vs_out.tc = tc;
                   gl_Position = proj_matrix * pos_vs;
               }
           )" };

        // Source code for fragment shader
        static const GLchar *fragment_shader_source[] {
            R"(
           #version 450 core

           layout (binding = 0) uniform sampler2D tex_object;

           in VS_OUT {
               vec2 tc;
           } fs_in;
           
            out vec4 color;

           void main() {
               color = texture(tex_object, fs_in.tc * vec2(3.0, 1.0));
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

        uniforms.mv_matrix = glGetUniformLocation(render_prog, "mv_matrix");
        uniforms.proj_matrix = glGetUniformLocation(render_prog, "proj_matrix");
    }

private:
    virtual void onKey(int key, int action) override {
        if (action) {
            switch (key) {
            case 'R':
                loadShaders();
                break;
            case 'T':
                tex_index++;
                if (tex_index > 1)
                    tex_index = 0;
                break;
            }
        }
    }

protected:
    GLuint render_prog;

    GLuint tex_object[2];
    GLuint tex_index;

    struct {
        GLint mv_matrix;
        GLint proj_matrix;
    } uniforms;

    sb7::object object;
};

DECLARE_MAIN(my_appliction);
