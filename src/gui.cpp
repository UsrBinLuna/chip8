#include "gui.h"
#include <iostream>

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 320;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* texture = nullptr;

bool initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
        return false;
    }
    window = SDL_CreateWindow("Chip8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_Log("Window could not be created! SDL_Error: %s", SDL_GetError());
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Renderer could not be created! SDL_Error: %s", SDL_GetError());
        return false;
    }
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, Chip8::DISPLAY_WIDTH, Chip8::DISPLAY_HEIGHT);
    if (!texture) {
        SDL_Log("Texture could not be created! SDL_Error: %s", SDL_GetError());
        return false;
    }
    return true;
}

void render(const Chip8& chip8) {
    uint32_t pixels[Chip8::DISPLAY_WIDTH * Chip8::DISPLAY_HEIGHT];

    std::cout << "Rendering Display: ";
    for (size_t i = 0; i < Chip8::DISPLAY_WIDTH * Chip8::DISPLAY_HEIGHT; i++) {
        pixels[i] = chip8.display[i] ? 0xFFFFFFFF : 0x000000;
    }

    SDL_UpdateTexture(texture, nullptr, pixels, Chip8::DISPLAY_WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);  // Update the screen
}



void cleanupSDL() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void handleInput(bool& running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
    }
}

void updateDisplay(const Chip8& chip8) {
    render(chip8);
}
