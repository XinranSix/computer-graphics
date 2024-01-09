`void glVertexAttrib*()`：用于设置顶点属性，表示一系列的函数，例如 `void glVertexAttrib4fv( GLuint index, const GLfloat* v)`，其中 `index` 参数表示顶点属性的 location，例如 `layout (location = 0) in vec4 position;` 表示 `offset` 的顶点属性的 loaction 为 `0`。顶点属性的 location 也可以使用函数 `void glGetAttribLocation(GLuint program, const GLchar* name)` 进行查询，该函数如果没查到会返回 -1。如果要查询上面的 `position` 顶点属性的 location，只需在第二个参数设置为 `"position"` 即可。

```glsl
layout (loaction = 0) in position;

void main() {
    // some code
}
```

## Shader 相关函数

- `glCreateShader()`: creates an empty shader object, ready to accept source code and be compiled.
- `glShaderSource()`: hands shader source code to the shader object so that it can keep a copy of it.
- `glCompileShader()`: compiles whatever source code is contained in the shader object.
- `glCreateProgram()`: creates a program object to which you can attach shader objects.
- `glAttachShader()`: attaches a shader object to a program object.
- `glLinkProgram()`: links all of the shader objects attached to a program objecttogether.
- `glDeleteShader()`: deletes a shader object. Once a shader has been linked into a program object, the program contains the binary code and the shader is no longerneeded.
- `glDeleteProgram(GLuint program)`: 

### 缓存

`void glCreateBuffers(GLsizei n, GLuint* buffers)`：创建 `n` 个缓存对象，其中 buffers 中的每一个元素称为每一个缓存的 name. 要创建一个缓存对象，let n = 1，buffers 可使用一个单独的 GLuint 类型的变量。当 n >= 2 时，buffers 可传递一个大小至少为 n 的 GLuint 类型的数组的首地址。这个函数实际上就是分配一个句柄。

`void glBindBuffer(GLenum target, GLuint buffer)`：将当前的 buffer 绑定到 target. 注意：同意时刻，一个 `target` 只能被一个 `buffer` 绑定，当前的绑定会使上一个绑定失效。

在使用缓存对象之前，需要为这些缓存对象分配 data store，可用于分配 data store 的函数有：

`void glBufferStorage(GLenum target, GLSizeiptr size, const void* data, GLbitfield flags)`

和

`void glNamedBufferStorage(GLuint buffer, GLSizeiptr size, const void* data, GLbitfield flags)`.

`glBufferStorage()` 的 `target` 参数和 `glBindBuffer` 的 `target` 参数含义一样，表示绑定的目标，例如 `GL_ARRAUY_BUFFER`。`glNamedBufferStorage()` 的 `buffer` 参数就是 `glCreateBuffer()` 获得的 buffer。`glBufferStorage` 和 `glBindBuffer` 的其他参数含义相同：

- `size`: 存储区域的大小，即这个 buffer 可用放多少数据，以 byte 为单位。
- `data`：传递数据来初始化这个 buffer，也可以传 `nullptr`，则这个 buffer 只会分配大小而不初始化，类似于 C 语言的 `malloc()` 函数。
- `flags`: 位图，告诉 OpenGL 我们会如何使用这个 buffer。它可以为这些值：

![Buffer Storage Flags](https://github.com/XinranSix/computer-graphics/assets/62458905/868d068f-b1de-4090-bdcc-49655939eaf2)

使用 `glBufferStorage()` 和 `glNamedBufferStorage()` 分配的之后，被分配的这个 buffer 的**大小和 flags 不可变，但数据可变**，要调整大小和 flag，只能先删除它如何创建一个新的 buffer，重新设置大小和 flag.


下面是一个创建并缓存的例子：

```cpp
GLuint buffer;

// Create a buffer
glCreateBuffers(1, &buffer);

// Specify the data store parameters for the buffer
glNamedBufferStorage(
    buffer,                                   // Name for the buffer
    1024 * 1024,                              // 1 MiB of space
    nullptr,                                  // No initial data
    GL_MAP_WRITE_BIT                         // Allow map for writing
);

// or
/*
glBufferStorage(
    GL_ARRAY_BUFFER,                                   // Name for the buffer
    1024 * 1024,                              // 1 MiB of space
    nullptr,                                  // No initial data
    GL_MAP_WRITE_BIT                         // Allow map for writing
);
// 这种方式直接将数据传递到 GL_ARRAY_BUFFER
*/

// Now bind it to the context using the GL_ARRAY_BUFFER binding point
glBindBuffer(GL_ARRAY_BUFFER, buffer);
```

> 注意：`GL_ARRAY_BUFFER` 表示顶点数组对象，用以存储顶点数据。

可以在分配之后为 buffer 指定数据，可用的函数有：

`glBufferSubData(GLenum target, GLintptr offset, GLSizeiptr size, const void* data)`

和 

`glNamedBufferSubData(GLuint buffer, GLintptr offset, GLSizeiptr size, const void* data)`

要使用这两个函数，记得在 `glBufferStorage()` 和 `glNamedBufferStorage()` 中 or 上 `GL_DYNAMIC_STORAGE_BIT`。这函数的参数：

- `offset`: 要传入的数据相对于 data 起始位置的 offset，单位为 byte.
- `size`: 要传入的数据的大小
- `data`: 要传入的数据

下面是使用 `glBufferSubData()` 更新缓存内容的一个例子：

```cpp
// The type used for names in OpenGL is GLuint
GLuint buffer;

// Create a buffer
glCreateBuffers(1, &buffer);

// Specify the data store parameters for the buffer
glNamedBufferStorage(
    buffer,                                   // Name for the buffer
    1024 * 1024,                              // 1 MiB of space
    nullptr,                                  // No initial data
    GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT // Allow map for writing
);

// Now bind it to the context using the GL_ARRAY_BUFFER binding point
glBindBuffer(GL_ARRAY_BUFFER, buffer);

// This is the data that we will place into the buffer object
static const float data[] {
    0.25,  -0.25, 0.5, 1.0, //
    -0.25, -0.25, 0.5, 1.0, //
    0.25,  0.25,  0.5, 1.0  //
};

// Put the data into the buffer at offset zero
glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(data), data);
// or
// glNamedBufferSubData(buffer, 0, sizeof(data), data);
```

另外一种传递数据的方式为：像 OpenGL 请求一个由缓存对象表示的指针，即 mapping the buffer.例子入下：

```cpp
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
```

相关函数有：

- `void* glMapBuffer(GLenum target, GLenum usage)`
- `void* glMapNamedBuffer(GLuint buffer, GLenum usage)`
- `void glUnmapBuffer(GLenum target)`
- `void glUnmapNamedBuffer(GLuint buffer)`
 
but，`glMapBuffer` 和 `glMapNamedBuffer` 的 `usage` 参数太大用处，不建议使用这两个函数，而是使用：

- `glMapBufferRange(GLenum target, GLintptr offset, GLSizeiptr length, GLbitfield access)`;
- `glMapNamedBufferRange(GLuint buffer, GLintptr offset, GLSizeiptr length, GLbitfield access)`

为什么叫 「Range」，因为它有 Range，其 Range 从 `offset` 开始，长度为 `length`，`access` 可为以下值：

![Buffer-Mapping Flags](https://github.com/XinranSix/computer-graphics/assets/62458905/81111dc5-95a2-4694-af43-5d4ddbef0648)


### Filling and Copying Data in Buffers

如果要放入缓存的数据是一个 constant value，可用使用下面这两个函数：

- `void glClearBufferSubData(GLenum target, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void* data)`
- `void glClearNamedBufferSubData(GLuint buffer, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void* data)`

这两个函数将数据转换为 `internalformat` 指定的格式，将转换之后的数据复制到 `offset` 和 `size` 指定的 data store 范围内，单位均为 byte。`format` 和 `type` 告诉 OpenGL 所指向的数据的相关信息。`format` 可以是 `GL_RED`、`GL_RG`、`GL_RGB` or `GL_RGAB`、分别为 1、2、3 和 4 通道。`type` 表示各分量得到数据类型，这些数据类型见下表。

![Basic OpenGL Type Tokens and Their Corresponding C Types](https://github.com/XinranSix/computer-graphics/assets/62458905/bbc6e00c-1e6b-47ff-b4d8-de901e1e3b4b)

数据发送给 GPU 之后，我们还可以在多个 buffer 之间共享数据，或将数据从一个 buffer copy 到另外一个 buffer。可使用下面这个函数：

- `void glCopyBufferSubData(GLenum readtarget, GLenum writetarget, GLintptr readoffset, GLintptr writetarget, GLsizeiptr size);`
- `void glCopyNamedBufferSubData(GLuint readBuffer, GLuint writeBuffer, GLintptr readoffset, GLintptr writetarget, GLsizeiptr size);`

对于 `glCopyBufferSubData()`，`readtarget` 和 `writetarget` 是我们希望拷贝数据的两个 buffer binding points。but，不能同时在两个 `GL_ARRAY_BUFFER` buffer binding points 之间进行 copy（其他的 buffer binding points 也一样）。如果要 copy，最好使用 OpenGL 专门提供的两个 buffer binding points：`GL_COPY_READ_BUFFER` 和 `GL_COPY_WRITE_BUFFER`，这样你不管什么操作都是 ok 的。或者直接使用 `glCopyNamedBufferSubData`，这个函数类似于 C 语言的 `memcpy`，其未定义行为和 `memcpy` 差不多。

## 使用缓冲为顶点着色器提供数据

在向 vertex shader 传递数据之前，我们要创建一个 vertex array object 来存储我们的顶点数组状态并将其绑定到上下文：

```cpp
GLuint vao;
glCreateVertexArray(1, &vao);
glBindVertexArray(vao);
```

- `void glVertexArrayVertexBuffer(GLuint vao, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride)`
  - `vao`: 要修改的 vertex array object
  - `bindingindex`: 顶点缓存的索引
  - `buffer`: 要绑定的缓存对象的 name
  - `offset`: 第一个顶点数据开始的位置
  - `stride`: 每个顶点之间的距离
- `void glVertexArrayAttribBinding(GLuint vao, GLuint attribindex, GLuint bindingindex)`
  - `vao`: 要绑定的 vertex array object
  - `attribindex`: 顶点属性的 location
  - `bindingindex`: 顶点缓存的索引，与 `glVertexArrayVertexBuffer` 的 `bindingindex` 意义相同。
- `void glVertexArrayAttribFormat(GLuint vao, GLuint attribindex, GLuint size, GLenum type, GLboolean normalized, GLuint relativeoffset)`
  - `vao`: 要修改的 vertex array object 
  - `attribindex`: 顶点属性的 location
  - `size`: 当前属性的分量数
  - `type`: 数据类型
  - `normalized`: 是否标准化
  - `relativeoffset`: 偏移量
- `void glEnableVertexArrayAttrib(GLuint vao, GLuint index)`
- `void glDisableVertexArrayAttrib(GLuint vao, GLuint index);`

ex1:

```cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

static const GLchar *vertex_shader_source[] {
    R"(
        #version 450 core

        layout (location = 0) in vec4 position;
        layout (location = 1) in vec4 color;

        out vec4 vs_color;

        void main() {
            gl_Position = position;
            vs_color = color;
        }
    )"
};

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

static const GLfloat data[] {
    0.25,  -0.25, 0.5, 1.0, 1.0, 0.0, 0.0, 1.0, //
    -0.25, -0.25, 0.5, 1.0, 0.0, 1.0, 0.0, 1.0, //
    0.25,  0.25,  0.5, 1.0, 0.0, 0.0, 1.0, 1.0  //
};

static const GLfloat color[] { 0.7137f, 0.7333f, 0.7686f, 1.0f };

int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window { glfwCreateWindow(800, 600, "OpenGL", nullptr,
                                          nullptr) };
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    GLuint vertex_shader, fragment_shader, program;

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

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    GLuint vao, vbo;

    glCreateVertexArrays(1, &vao);

    glCreateBuffers(1, &vbo);

    glNamedBufferStorage(vao, sizeof(data), data, GL_MAP_WRITE_BIT);
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(GLfloat) * 8);

    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayAttribFormat(vao, 0, 4, GL_FLOAT, GL_FALSE, 0);
    glEnableVertexArrayAttrib(vao, 0);

    glVertexArrayAttribBinding(vao, 1, 0);
    glVertexArrayAttribFormat(vao, 1, 4, GL_FLOAT, GL_FALSE,
                              sizeof(GLfloat) * 4);
    glEnableVertexArrayAttrib(vao, 1);

    while (!glfwWindowShouldClose(window)) {
        glClearBufferfv(GL_COLOR, 0, color);

        glBindVertexArray(vao);
        glUseProgram(program);

        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteProgram(program);
    glfwTerminate();

    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

```

ex2:

```cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

static const GLchar *vertex_shader_source[] {
    R"(
        #version 450 core

        layout (location = 0) in vec3 position;
        layout (location = 1) in vec3 color;

        out vec4 vs_color;

        void main() {
            gl_Position = vec4(position, 1.0);
            vs_color = vec4(color, 1.0);
        }
    )"
};

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

static const GLfloat positions[] {
    0.25,  -0.25, 0.5, //
    -0.25, -0.25, 0.5, //
    0.25,  0.25,  0.5  //
};

static const GLfloat colors[] {
    1.0, 0.0, 0.0, //
    0.0, 1.0, 0.0, //
    0.0, 0.0, 1.0  //
};

static const GLfloat color[] { 0.7137f, 0.7333f, 0.7686f, 1.0f };

int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window { glfwCreateWindow(800, 600, "OpenGL", nullptr,
                                          nullptr) };
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    GLuint vertex_shader, fragment_shader, program;

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

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    GLuint vao, vbo[2];

    glCreateVertexArrays(1, &vao);
    glCreateBuffers(2, vbo);

    glNamedBufferStorage(vbo[0], sizeof(positions), positions, 0);
    glNamedBufferStorage(vbo[1], sizeof(colors), colors, 0);

    glVertexArrayVertexBuffer(vao, 0, vbo[0], 0, sizeof(GLfloat) * 3);
    glVertexArrayVertexBuffer(vao, 1, vbo[1], 0, sizeof(GLfloat) * 3);

    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayAttribBinding(vao, 1, 1);

    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, 0);

    glEnableVertexArrayAttrib(vao, 0);
    glEnableVertexArrayAttrib(vao, 1);

    while (!glfwWindowShouldClose(window)) {
        glClearBufferfv(GL_COLOR, 0, color);

        glBindVertexArray(vao);
        glUseProgram(program);

        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteProgram(program);
    glfwTerminate();

    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

```

ex3:

```cpp
#include <cstddef>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

struct vertex {
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat r;
    GLfloat g;
    GLfloat b;
};

static const GLchar *vertex_shader_source[] {
    R"(
        #version 450 core

        layout (location = 0) in vec3 position;
        layout (location = 1) in vec3 color;

        out vec4 vs_color;

        void main() {
            gl_Position = vec4(position, 1.0);
            vs_color = vec4(color, 1.0);
        }
    )"
};

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

static const vertex vertices[] {
    { 0.25, -0.25, 0.5, 1.0, 0.0, 0.0 },  //
    { -0.25, -0.25, 0.5, 0.0, 1.0, 0.0 }, //
    { 0.25, 0.25, 0.5, 0.0, 0.0, 1.0 },   //
};
static const GLfloat color[] { 0.7137f, 0.7333f, 0.7686f, 1.0f };

int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window { glfwCreateWindow(800, 600, "OpenGL", nullptr,
                                          nullptr) };
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    GLuint vertex_shader, fragment_shader, program;

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

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    GLuint vao, vbo;

    glCreateVertexArrays(1, &vao);
    glCreateBuffers(1, &vbo);

    glNamedBufferStorage(vbo, sizeof(vertices), vertices, GL_MAP_WRITE_BIT);

    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(vertex));

    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayAttribBinding(vao, 1, 0);

    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE,
                              offsetof(vertex, x));
    glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE,
                              offsetof(vertex, r));

    glEnableVertexArrayAttrib(vao, 0);
    glEnableVertexArrayAttrib(vao, 1);

    while (!glfwWindowShouldClose(window)) {
        glClearBufferfv(GL_COLOR, 0, color);

        glBindVertexArray(vao);
        glUseProgram(program);

        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteProgram(program);
    glfwTerminate();

    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

```


或者可以使用下面这些函数：

- `void glEnableVertexAttribArray(GLuint index)`
- `void glDisableVertexAttribArray(GLuint index)`
- `void glBufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage)`
- `void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer)`;

![image](https://github.com/XinranSix/computer-graphics/assets/62458905/f89dc2ec-9435-4e80-954f-737ae1c09b09)


用这几个函数改写上面的 ex1 和 ex3：

ex1: 

```cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

static const GLchar *vertex_shader_source[] {
    R"(
        #version 450 core

        layout (location = 0) in vec4 position;
        layout (location = 1) in vec4 color;

        out vec4 vs_color;

        void main() {
            gl_Position = position;
            vs_color = color;
        }
    )"
};

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

static const GLfloat data[] {
    0.25,  -0.25, 0.5, 1.0, 1.0, 0.0, 0.0, 1.0, //
    -0.25, -0.25, 0.5, 1.0, 0.0, 1.0, 0.0, 1.0, //
    0.25,  0.25,  0.5, 1.0, 0.0, 0.0, 1.0, 1.0  //
};

static const GLfloat color[] { 0.7137f, 0.7333f, 0.7686f, 1.0f };

int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window { glfwCreateWindow(800, 600, "OpenGL", nullptr,
                                          nullptr) };
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    GLuint vertex_shader, fragment_shader, program;

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

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    GLuint vao, vbo;

    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glCreateBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8,
                          (void *)(sizeof(GLfloat) * 4));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    while (!glfwWindowShouldClose(window)) {
        glClearBufferfv(GL_COLOR, 0, color);

        glBindVertexArray(vao);
        glUseProgram(program);

        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteProgram(program);
    glfwTerminate();

    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

```

ex2: 

```cpp
#include <cstddef>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

struct vertex {
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat r;
    GLfloat g;
    GLfloat b;
};

static const GLchar *vertex_shader_source[] {
    R"(
        #version 450 core

        layout (location = 0) in vec3 position;
        layout (location = 1) in vec3 color;

        out vec4 vs_color;

        void main() {
            gl_Position = vec4(position, 1.0);
            vs_color = vec4(color, 1.0);
        }
    )"
};

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

static const vertex vertices[] {
    { 0.25, -0.25, 0.5, 1.0, 0.0, 0.0 },  //
    { -0.25, -0.25, 0.5, 0.0, 1.0, 0.0 }, //
    { 0.25, 0.25, 0.5, 0.0, 0.0, 1.0 },   //
};
static const GLfloat color[] { 0.7137f, 0.7333f, 0.7686f, 1.0f };

int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window { glfwCreateWindow(800, 600, "OpenGL", nullptr,
                                          nullptr) };
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    GLuint vertex_shader, fragment_shader, program;

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

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    GLuint vao, vbo;

    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glCreateBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex),
                          (void *)(offsetof(vertex, x)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex),
                          (void *)(offsetof(vertex, r)));

    glEnableVertexAttribArray(1);

    while (!glfwWindowShouldClose(window)) {
        glClearBufferfv(GL_COLOR, 0, color);

        glBindVertexArray(vao);
        glUseProgram(program);

        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteProgram(program);
    glfwTerminate();

    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

```

