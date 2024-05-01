//
// Created by captain on 2021/3/25.
//

// 顶点着色器代码
static const char *vertex_shader_text = R"(
    #version 450 core

    uniform mat4 u_mvp;

    in (layout = 0) vec3 a_pos;
    in (layout = 1) vec4 a_color;

    out vec4 v_color;

    void main() {
        gl_Position = u_mvp * vec4(a_pos, 1.0);
        v_color = a_color;
    }
)";

// 片段着色器代码
static const char *fragment_shader_text = R"(
    #version 450

    in vec4 v_color;

    void main() {
        gl_FragColor = v_color;
    }
)";
