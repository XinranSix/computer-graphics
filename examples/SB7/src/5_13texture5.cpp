#include <object.h>
#include <sb7.h>
#include <sb7ktx.h>
#include <shader.h>
#include <vmath.h>

#include <iostream>


// Source code for vertex shader
static const GLchar *vertex_shader_source[]{
    R"(
        #version 450 core

        uniform vec2 offset;
 
        out vec2 tex_coord;

        void main() {
            const vec4 vertices[] = vec4[](
                vec4(-0.45, -0.45, 0.5, 1.0),
                vec4( 0.45, -0.45, 0.5, 1.0),
                vec4(-0.45,  0.45, 0.5, 1.0),
                vec4( 0.45,  0.45, 0.5, 1.0)
            );

            gl_Position = vertices[gl_VertexID] + vec4(offset, 0.0, 0.0);
            tex_coord = vertices[gl_VertexID].xy * 3.0 + vec2(0.45 * 3);
        }
            )"};

// Source code for fragment shader
static const GLchar *fragment_shader_source[]{
    R"(
        #version 450 core

        uniform sampler2D s;

        out vec4 color;

        in vec2 tex_coord;

        void main() {
            color = texture(s, tex_coord);
        }
            )"};

class my_appliction : public sb7::application {
   public:
   protected:
    void init() override {
        static const char title[] = "OpenGL SuperBible - Texture Wrap Modes";

        sb7::application::init();

        memcpy(info.title, title, sizeof(title));
    }

    void startup() override {
        glGenTextures(1, &texture);
        sb7::ktx::file::load("asset/sb7/textures/rightarrows.ktx", texture);

        glBindTexture(GL_TEXTURE_2D, texture);

        loadShaders();

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
    }

    void render(double currentTime) override {
        static const GLfloat green[] = {0.0f, 0.1f, 0.0f, 1.0f};
        static const GLfloat yellow[] = {0.4f, 0.4f, 0.0f, 1.0f};
        glClearBufferfv(GL_COLOR, 0, green);

        static const GLenum wrapmodes[] = {GL_CLAMP_TO_EDGE, GL_REPEAT, GL_CLAMP_TO_BORDER, GL_MIRRORED_REPEAT};
        static const float offsets[] = {-0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f};

        glUseProgram(program);

        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, yellow);

        for (int i = 0; i < 4; i++) {
            glUniform2fv(0, 1, &offsets[i * 2]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapmodes[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapmodes[i]);

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
    }

    void shutdown(void) override {
        glDeleteProgram(program);
        glDeleteVertexArrays(1, &vao);
        glDeleteTextures(1, &texture);
    }

   private:
    void loadShaders() {
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
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        // Create and compile fragment shader
        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, fragment_shader_source, nullptr);
        glCompileShader(fragment_shader);
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragment_shader, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        // Create program, attach shaders to it, and link it
        program = glCreateProgram();
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);
        glLinkProgram(program);
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(program, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        // Delete the shaders as the program has them now
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }

   private:
   protected:
    GLuint texture;
    GLuint program;
    GLuint vao;
};

DECLARE_MAIN(my_appliction);
