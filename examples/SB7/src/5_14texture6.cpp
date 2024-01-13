#include <object.h>
#include <sb7.h>
#include <sb7ktx.h>
#include <shader.h>
#include <vmath.h>

#include <iostream>

// Random number generator
static unsigned int seed = 0x13371337;

static inline float random_float() {
    float res;
    unsigned int tmp;

    seed *= 16807;

    tmp = seed ^ (seed >> 4) ^ (seed << 15);

    *((unsigned int *)&res) = (tmp >> 9) | 0x3F800000;

    return (res - 1.0f);
}


// Source code for vertex shader
static const GLchar *vertex_shader_source[] {
    R"(
        #version 450 core

        layout (location = 0) in int alien_index;

        out VS_OUT {
            flat int alien;
            vec2 tc;
        } vs_out;

        struct droplet_t {
            float x_offset;
            float y_offset;
            float orientation;
            float unused;
        };

        layout (std140) uniform droplets {
            droplet_t droplet[256];
        };

        void main() {
            const vec2[4] position = vec2[4](
                vec2(-0.5, -0.5),
                vec2( 0.5, -0.5),
                vec2(-0.5,  0.5),
                vec2( 0.5,  0.5)
            );
            
            vs_out.tc = position[gl_VertexID].xy + vec2(0.5);
            float co = cos(droplet[alien_index].orientation);
            float so = sin(droplet[alien_index].orientation);
            mat2 rot = mat2(
                vec2(co, so),
                vec2(-so, co)
            );
            vec2 pos = 0.25 * rot * position[gl_VertexID];
            gl_Position = vec4(
                pos.x + droplet[alien_index].x_offset,
                pos.y + droplet[alien_index].y_offset,
                0.5, 1.0
            );
            vs_out.alien = alien_index % 64;
        }
            )"
};

// Source code for fragment shader
static const GLchar *fragment_shader_source[] {
    R"(
        #version 450 core

        layout (location = 0) out vec4 color;

        in VS_OUT {
            flat int alien;
            vec2 tc;
        } fs_in;

        uniform sampler2DArray tex_aliens;

        void main() {
            color = texture(tex_aliens, vec3(fs_in.tc, float(fs_in.alien)));
        }
            )"
};


class my_appliction : public sb7::application {
public:
protected:
    void init() override {
        static const char title[] = "OpenGL SuperBible - Texture Wrap Modes";

        sb7::application::init();

        memcpy(info.title, title, sizeof(title));
    }

    void startup() override {
        loadShaders();
        glGenVertexArrays(1, &render_vao);
        glBindVertexArray(render_vao);

        tex_alien_array = sb7::ktx::file::load("asset/sb7/textures/aliens.ktx");
        glBindTexture(GL_TEXTURE_2D_ARRAY, tex_alien_array);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        glCreateBuffers(1, &rain_buffer);
        glBindBuffer(GL_UNIFORM_BUFFER, rain_buffer);
        glBufferData(GL_UNIFORM_BUFFER, 256 * sizeof(vmath::vec4), nullptr, GL_DYNAMIC_DRAW);

        for (int i {}; i < 256; ++i) {
            droplet_x_offset[i] = random_float() * 2.0f - 1.0f;
            droplet_rot_speed[i] = (random_float() + 0.5f) * ((i & 1) ? -3.0f : 3.0f);
            droplet_fall_speed[i] = random_float() + 0.2f;
        }

        glBindVertexArray(render_vao);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void render(double currentTime) override {
        static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        float t = (float)currentTime;

        glViewport(0, 0, info.windowWidth, info.windowHeight);
        glClearBufferfv(GL_COLOR, 0, black);

        glUseProgram(render_prog);

        glBindBufferBase(GL_UNIFORM_BUFFER, 0, rain_buffer);
        vmath::vec4 *droplet = (vmath::vec4 *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, 256 * sizeof(vmath::vec4),
                                                               GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

        for (int i = 0; i < 256; i++) {
            droplet[i][0] = droplet_x_offset[i];
            droplet[i][1] = 2.0f - fmodf((t + float(i)) * droplet_fall_speed[i], 4.31f);
            droplet[i][2] = t * droplet_rot_speed[i];
            droplet[i][3] = 0.0f;
        }
        glUnmapBuffer(GL_UNIFORM_BUFFER);

        int alien_index;
        for (alien_index = 0; alien_index < 256; alien_index++) {
            glVertexAttribI1i(0, alien_index);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
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
        render_prog = glCreateProgram();
        glAttachShader(render_prog, vertex_shader);
        glAttachShader(render_prog, fragment_shader);
        glLinkProgram(render_prog);
        glGetProgramiv(render_prog, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(render_prog, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        // Delete the shaders as the program has them now
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }

protected:
    GLuint render_prog;
    GLuint render_vao;

    GLuint tex_alien_array;
    GLuint rain_buffer;

    float droplet_x_offset[256];
    float droplet_rot_speed[256];
    float droplet_fall_speed[256];
};

DECLARE_MAIN(my_appliction);
