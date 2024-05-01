#include "SDL2/SDL.h"
#include "glad/glad.h"
#include <iostream>

// 窗口宽度和高度
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

SDL_Window *window {};
SDL_GLContext context {};

void render() {
    // 清除颜色缓冲区
    glClear(GL_COLOR_BUFFER_BIT);

    // 绘制一个彩色三角形
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f); // 红色
    glVertex2f(-0.5f, -0.5f);
    glColor3f(0.0f, 1.0f, 0.0f); // 绿色
    glVertex2f(0.5f, -0.5f);
    glColor3f(0.0f, 0.0f, 1.0f); // 蓝色
    glVertex2f(0.0f, 0.5f);
    glEnd();

    // 交换前后缓冲区
    SDL_GL_SwapWindow(window);
}

int main(int argc, char *args[]) {
    // 初始化SDL2
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    // 创建窗口
    window = SDL_CreateWindow("SDL2 OpenGL Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH,
                              WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // 创建OpenGL上下文
    context = SDL_GL_CreateContext(window);
    if (context == nullptr) {
        std::cerr << "OpenGL context could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // 初始化GLAD
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // 初始化OpenGL
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // 设置清除颜色为黑色

    // 主循环标志
    bool quit = false;
    SDL_Event e;

    // 主循环
    while (!quit) {
        // 处理事件
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // 渲染场景
        render();
    }

    // 清理资源
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
