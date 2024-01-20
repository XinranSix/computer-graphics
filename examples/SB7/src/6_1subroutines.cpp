#include <object.h>
#include <sb7.h>
#include <sb7ktx.h>
#include <shader.h>
#include <vmath.h>

#include <iostream>


// Source code for vertex shader
static const GLchar *vertex_shader_source[] {
    R"(
        #version 450 core

        void main() {
            vec4 vertices[4] = vec4[4](
                vec4(-1.0, -1.0, 0.5, 1.0),
                vec4( 1.0, -1.0, 0.5, 1.0),
                vec4(-1.0,  1.0, 0.5, 1.0),
                vec4( 1.0,  1.0, 0.5, 1.0)
            );

            gl_Position = vertices[gl_VertexID];
        }
    )"
};

// Source code for fragment shader
static const GLchar *fragment_shader_source[] {
    R"(
        #version 450 core

        subroutine vec4 sub_mySubroutine(vec4 param1);

        subroutine (sub_mySubroutine)
        vec4 myFunction1(vec4 param1) {
            return param1 * vec4(1.0, 0.25, 0.25, 1.0);
        }

        subroutine (sub_mySubroutine)
        vec4 myFunction2(vec2 param1) {
            return param1 * vec4(0.25, 0.25, 1.0, 1.0);
        }

        subroutine uniform sub_mySubroutine mySubroutineUniform;

        out vec4 color;

        void main() {
            color = mySubroutineUniform(vec4(1.0));
        }
    )"
};


class my_appliction : public sb7::application {
public:
public:
    my_appliction() : render_program { 0 } {
    }

    void startup();

    void render(double currentTime);

protected:
    void init() {
        static const char title[] = "OpenGL SuperBible - Shader Subroutines";

        sb7::application::init();

        memcpy(info.title, title, sizeof(title));
    }

    void loadShaders();
    void onKey(int key, int action);

protected:
    GLuint render_program;

    GLuint vao;

    GLuint subroutines[2];

    struct {
        GLint subroutine1;
    } uniforms;
};


void my_appliction::startup() {
    loadShaders();

    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
}

void my_appliction::render(double currentTime) {
    int i { static_cast<int>(currentTime) };

    glUseProgram(render_program);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &subroutines[i & 1]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void my_appliction::onKey(int key, int action) {
    if (action) {
        switch (key) {
        case 'R':
            loadShaders();
            break;
        }
    }
}

void my_appliction::loadShaders() {
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
    render_program = glCreateProgram();
    glAttachShader(render_program, vertex_shader);
    glAttachShader(render_program, fragment_shader);
    glLinkProgram(render_program);
    glGetProgramiv(render_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(render_program, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    subroutines[0] = glGetSubroutineIndex(render_program, GL_FRAGMENT_SHADER, "myFunction1");
    subroutines[1] = glGetSubroutineIndex(render_program, GL_FRAGMENT_SHADER, "myFunction2");

    uniforms.subroutine1 = glGetSubroutineUniformLocation(render_program, GL_FRAGMENT_SHADER, "mySubroutineUniform");

    // Delete the shaders as the program has them now
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}


DECLARE_MAIN(my_appliction);
