#include <iostream>
#include <SDL2/SDL.h>

const int WIDTH = 800;
const int HEIGHT = 600;
const int MAX_ITERATIONS = 1000;
const double ZOOM_FACTOR = 1.1;

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;

void drawMandelbrot(double zoom, double moveX, double moveY, int iterations) {
    for (int x = 0; x < WIDTH; ++x) {
        for (int y = 0; y < HEIGHT; ++y) {
            double zx = 0.0;
            double zy = 0.0;
            double cx = (x - WIDTH / 2.0) * zoom + moveX;
            double cy = (y - HEIGHT / 2.0) * zoom + moveY;

            int currentIterations = 0;

            while (zx * zx + zy * zy < 4.0 && currentIterations < iterations) {
                double temp = zx * zx - zy * zy + cx;
                zy = 2.0 * zx * zy + cy;
                zx = temp;
                currentIterations++;
            }

            if (currentIterations == iterations) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, currentIterations % 256, currentIterations % 256,
                                       currentIterations % 256, 255);
            }

            SDL_RenderDrawPoint(renderer, x, y);
        }
    }

    SDL_RenderPresent(renderer);
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    window = SDL_CreateWindow("Mandelbrot Set", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT,
                              SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
        return -1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cout << "Failed to create renderer: " << SDL_GetError() << std::endl;
        return -1;
    }

    bool running = true;
    SDL_Event event;

    double zoom = 0.005;
    double moveX = -0.5;
    double moveY = 0.0;
    int iterations = MAX_ITERATIONS;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_MOUSEWHEEL) {
                if (event.wheel.y > 0) {
                    zoom /= ZOOM_FACTOR;
                    iterations *= 2;
                }
            } else if (event.type == SDL_MOUSEWHEEL) {
                if (event.wheel.y > 0) {
                    zoom /= ZOOM_FACTOR;
                    iterations *= 2;
                } else if (event.wheel.y < 0) {
                    zoom *= ZOOM_FACTOR;
                    iterations /= 2;
                }
            }
        }
        drawMandelbrot(zoom, moveX, moveY, iterations);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
