#include <SDL2/SDL.h>
#include <stdio.h>
#include <vector>
#include <cmath>
#include <string>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define ZOOM 1

#define RANGE 16
#define THICKNESS 10
#define SCALE 80

void renderGraph(SDL_Renderer *renderer, int zoom, int width, int height) {
    zoom = zoom - 1;
    zoom = (zoom%20 + 20)%20;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 50, 0, 50, 255);
    for (int i = width / 2; i < width; i = i + (20 + zoom)) {
        SDL_RenderDrawLine(renderer, i, 0, i, height);
    }
    for (int i = width / 2; i > 0; i = i - (20 + zoom)) {
        SDL_RenderDrawLine(renderer, i, 0, i, height);
    }
    for (int i = height / 2; i < height; i = i + (20 + zoom)) {
        SDL_RenderDrawLine(renderer, 0, i, width, i);
    }
    for (int i = height / 2; i > 0; i = i - (20 + zoom)) {
        SDL_RenderDrawLine(renderer, 0, i, width, i);
    }

    SDL_SetRenderDrawColor(renderer, 125, 0, 125, 255);
    for (int i = width / 2; i < width; i = i + (80 + 4 * zoom)) {
        SDL_RenderDrawLine(renderer, i, 0, i, height);
    }
    for (int i = width / 2; i > 0; i = i - (80 + 4 * zoom)) {
        SDL_RenderDrawLine(renderer, i, 0, i, height);
    }
    for (int i = height / 2; i < height; i = i + (80 + 4 * zoom)) {
        SDL_RenderDrawLine(renderer, 0, i, width, i);
    }
    for (int i = height / 2; i > 0; i = i - (80 + 4 * zoom)) {
        SDL_RenderDrawLine(renderer, 0, i, width, i);
    }

    SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
    SDL_RenderDrawLine(renderer, 0, height / 2, width, height / 2);
    SDL_RenderDrawLine(renderer, width / 2, 0, width / 2, height);
}

void renderFunction(SDL_Renderer *renderer, double wScale, double hScale, int width, int height) {
    double aWidth = width / wScale;
    double aHeight = height / hScale;

    std::vector<SDL_Point> points;
    double y = 0.0;
    double perp = 0.0;
    double angle = 0.0;
    for (int j = 1 - THICKNESS; j < THICKNESS; j++) {
        for (int i = -(RANGE * SCALE / 2); i <= RANGE * SCALE / 2; i++) {
            y = sin((double)i / SCALE) * SCALE;
            perp = -1.0 / cos((double)i / SCALE);
            angle = atan(perp);

            SDL_Point point;
            if (i != 0) {
                point = {i + (int)(j * cos(angle)) + (int)aWidth / 2,
                        -((int)y + (int)(j * sin(angle))) + (int)aHeight / 2};
            } else {
                point = {i + (int)aWidth / 2,
                        -((int)y + j) + (int)aHeight / 2};
            }
            points.emplace_back(point);
        }
    }
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawPoints(renderer, points.data(), points.size());
}

int main (int argc, char** argv) {
    int screenWidth = SCREEN_WIDTH;
    int screenHeight = SCREEN_HEIGHT;
    int zoom = ZOOM;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Error: SDL failed to initialize\nSDL Error: '%s'\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("SDL test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!window) {
        printf("Error: Failed to open window\nSDL Error: '%s'\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Error: Failed to create renderer\nSDL Error: '%s'\n", SDL_GetError());
        return 1;
    }

    renderGraph(renderer, zoom, screenWidth, screenHeight);
    
    SDL_RenderPresent(renderer);

    bool running = true;
    std::string title = "";
    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    double deltaTime = 0;
    while (running) {
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        deltaTime = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_MOUSEWHEEL:
                    if (event.wheel.y > 0) {
                        zoom++;
                    } else if (event.wheel.y < 0) {
                        zoom--;
                    }
                    break;
                default:
                    break;
            }
        }

        renderGraph(renderer, zoom, screenWidth, screenHeight);
    
        SDL_RenderPresent(renderer);
        
        title = "DeltaTime: " + std::to_string((Uint32)deltaTime) + "ms" + "  Scale: " + std::to_string((zoom + 20) / 20) + "x";
        
        SDL_SetWindowTitle(window, title.c_str());
    }

    return 0;
}