#ifndef GUI_H
#define GUI_H

#include "definitions.h"
#include <atomic>

extern bool paused;

bool initSDL();
void render(const Chip8& chip8);
void cleanupSDL();
void renderDebugInfo(const Chip8& chip8);
void renderDebugWindow();
void handleInput(std::atomic<bool>& running, Chip8& chip8);
void updateDisplay(const Chip8& chip8);

#endif // GUI_H