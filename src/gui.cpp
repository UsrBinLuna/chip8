#include "gui.h"
#include "font_data.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <atomic>
#include <SDL2/SDL_ttf.h>

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 320;
const int DEBUG_WINDOW_WIDTH = 400;
const int DEBUG_WINDOW_HEIGHT = 600;

static std::string lastDebugText = "";
static SDL_Texture* debugTextTexture = nullptr;
static SDL_Texture* debugWindowTexture = nullptr;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* texture = nullptr;
SDL_Window* debugWindow = nullptr;
SDL_Renderer* debugRenderer = nullptr;
TTF_Font* font = nullptr;

bool paused = false;

bool initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
        return false;
    }

    SDL_EventState(SDL_QUIT, SDL_ENABLE);

    if (TTF_Init() == -1) {
        SDL_Log("TTF_Init failed: %s", TTF_GetError());
        return false;
    }

    // font is stored as byte array in font_data.h, CascadiaMono
    SDL_RWops* fontMemory = SDL_RWFromMem(font_ttf, font_ttf_len);
    font = TTF_OpenFontRW(fontMemory, 1, 16);
    if (!font) {
        SDL_Log("Failed to load embedded font: %s", TTF_GetError());
        return false;
    }


    if (!font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        return false;
    }

    window = SDL_CreateWindow("Chip8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    debugWindow = SDL_CreateWindow("Chip8 Debug", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, DEBUG_WINDOW_WIDTH, DEBUG_WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    if (!window || !debugWindow) {
        SDL_Log("Window could not be created! SDL_Error: %s", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    debugRenderer = SDL_CreateRenderer(debugWindow, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer || !debugRenderer) {
        SDL_Log("Renderer could not be created! SDL_Error: %s", SDL_GetError());
        return false;
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, Chip8::DISPLAY_WIDTH, Chip8::DISPLAY_HEIGHT);
    return true;
}

void render(const Chip8& chip8) {
    if (!chip8.displayChanged) return;
    uint32_t pixels[Chip8::DISPLAY_WIDTH * Chip8::DISPLAY_HEIGHT];

    for (size_t i = 0; i < Chip8::DISPLAY_WIDTH * Chip8::DISPLAY_HEIGHT; i++) {
        pixels[i] = chip8.display[i] ? 0xFFFFFFFF : 0x000000;
    }

    SDL_UpdateTexture(texture, nullptr, pixels, Chip8::DISPLAY_WIDTH * sizeof(uint32_t));

    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer); // Update only when necessary

    // Reset displayChanged flag to avoid redundant rendering
    chip8.displayChanged = false;
}



void renderText(SDL_Renderer* renderer, const std::string& text, int x, int y) {
    if (!font) return;

    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), white, DEBUG_WINDOW_WIDTH);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dstRect = {x, y, surface->w, surface->h};

    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
    SDL_DestroyTexture(texture);
}

void renderDebugInfo(const Chip8& chip8) {
    std::stringstream debugText;
    debugText << "PC: 0x" << std::hex << std::setw(3) << std::setfill('0') << chip8.pc << "\n";
    debugText << "I: 0x" << std::hex << std::setw(3) << std::setfill('0') << chip8.i_reg << "\n";

    for (int i = 0; i < 16; i++) {
        debugText << "V" << std::dec << i << ": 0x"
                  << std::hex << std::setw(2) << std::setfill('0')
                  << (int)chip8.v_reg[i] << " ";
        if (i % 4 == 3) debugText << "\n";
    }
    debugText << "\nStack:\n";
    for (size_t i = 0; i < chip8.stack.size(); i++) {
        debugText << i << ": 0x"
                  << std::hex << std::setw(3) << std::setfill('0')
                  << chip8.stack[i] << "\n";
    }

    for (size_t i = 0; i < chip8.keypad.size(); i++) {
        debugText << "Keypad: 0x"
                  << std::hex << i << ": " << chip8.keypad[i] << "\n";
    }

    std::string newDebugText = debugText.str();
    if (newDebugText == lastDebugText) return;
    lastDebugText = newDebugText;

    if (debugTextTexture) SDL_DestroyTexture(debugTextTexture);
    if (debugWindowTexture) SDL_DestroyTexture(debugWindowTexture);

    SDL_Surface* surface = TTF_RenderUTF8_Blended_Wrapped(font, newDebugText.c_str(), {255, 255, 255, 255}, DEBUG_WINDOW_WIDTH);
    if (!surface) return;

    debugTextTexture = SDL_CreateTextureFromSurface(debugRenderer, surface);
    debugWindowTexture = SDL_CreateTexture(debugRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, DEBUG_WINDOW_WIDTH, DEBUG_WINDOW_HEIGHT);

    SDL_SetRenderTarget(debugRenderer, debugWindowTexture);
    SDL_SetRenderDrawColor(debugRenderer, 0, 0, 0, 255);
    SDL_RenderClear(debugRenderer);

    SDL_Rect dstRect = {10, 10, surface->w, surface->h};
    SDL_RenderCopy(debugRenderer, debugTextTexture, nullptr, &dstRect);

    SDL_SetRenderTarget(debugRenderer, nullptr);
    SDL_FreeSurface(surface);
}

void renderDebugWindow() {
    if (!debugWindowTexture) return;

    SDL_SetRenderDrawColor(debugRenderer, 0, 0, 0, 255);
    SDL_RenderClear(debugRenderer);

    SDL_RenderCopy(debugRenderer, debugWindowTexture, nullptr, nullptr);
    SDL_RenderPresent(debugRenderer);
}

void cleanupSDL() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyRenderer(debugRenderer);
    SDL_DestroyWindow(window);
    SDL_DestroyWindow(debugWindow);
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    TTF_Quit();
    SDL_Quit();
}

void handleInput(std::atomic<bool>& running, Chip8& chip8) {
    static bool prevKeypad[16] = {false};
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            std::cout << "Received SDL_QUIT event. Exiting...\n";
            running.store(false);
            return;
        } else if (event.type == SDL_WINDOWEVENT) {
            if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                std::cout << "SDL_WINDOWEVENT_CLOSE received!" << std::endl;
                running.store(false);
            }
        } else if (event.type == SDL_KEYDOWN) {
            SDL_Scancode scancode = event.key.keysym.scancode;
            if (event.key.keysym.sym == SDLK_SPACE) {
                paused = !paused;
                std::cout << (paused ? "Emulator paused.\n" : "Emulator resumed.\n");
            }

            // scancodes
            switch(scancode) {
                case SDL_SCANCODE_1: chip8.keypad[0x1] = true; break;
                case SDL_SCANCODE_2: chip8.keypad[0x2] = true; break;
                case SDL_SCANCODE_3: chip8.keypad[0x3] = true; break;
                case SDL_SCANCODE_4: chip8.keypad[0xC] = true; break;
                case SDL_SCANCODE_Q: chip8.keypad[0x4] = true; break;
                case SDL_SCANCODE_W: chip8.keypad[0x5] = true; break;
                case SDL_SCANCODE_E: chip8.keypad[0x6] = true; break;
                case SDL_SCANCODE_R: chip8.keypad[0xD] = true; break;
                case SDL_SCANCODE_A: chip8.keypad[0x7] = true; break;
                case SDL_SCANCODE_S: chip8.keypad[0x8] = true; break;
                case SDL_SCANCODE_D: chip8.keypad[0x9] = true; break;
                case SDL_SCANCODE_F: chip8.keypad[0xE] = true; break;
                case SDL_SCANCODE_Z: chip8.keypad[0xA] = true; break;
                case SDL_SCANCODE_X: chip8.keypad[0x0] = true; break;
                case SDL_SCANCODE_C: chip8.keypad[0xB] = true; break;
                case SDL_SCANCODE_V: chip8.keypad[0xF] = true; break;
            }
        } else if (event.type == SDL_KEYUP) {
            SDL_Scancode scancode = event.key.keysym.scancode;

            // scancodes
            switch(scancode) {
                case SDL_SCANCODE_1: chip8.keypad[0x1] = false; break;
                case SDL_SCANCODE_2: chip8.keypad[0x2] = false; break;
                case SDL_SCANCODE_3: chip8.keypad[0x3] = false; break;
                case SDL_SCANCODE_4: chip8.keypad[0xC] = false; break;
                case SDL_SCANCODE_Q: chip8.keypad[0x4] = false; break;
                case SDL_SCANCODE_W: chip8.keypad[0x5] = false; break;
                case SDL_SCANCODE_E: chip8.keypad[0x6] = false; break;
                case SDL_SCANCODE_R: chip8.keypad[0xD] = false; break;
                case SDL_SCANCODE_A: chip8.keypad[0x7] = false; break;
                case SDL_SCANCODE_S: chip8.keypad[0x8] = false; break;
                case SDL_SCANCODE_D: chip8.keypad[0x9] = false; break;
                case SDL_SCANCODE_F: chip8.keypad[0xE] = false; break;
                case SDL_SCANCODE_Z: chip8.keypad[0xA] = false; break;
                case SDL_SCANCODE_X: chip8.keypad[0x0] = false; break;
                case SDL_SCANCODE_C: chip8.keypad[0xB] = false; break;
                case SDL_SCANCODE_V: chip8.keypad[0xF] = false; break;
            }
        }
    }
}


void updateDisplay(const Chip8& chip8) {
    render(chip8);
}
