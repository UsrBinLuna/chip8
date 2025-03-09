#ifndef GUI_H
#define GUI_H

#include <SDL2/SDL.h>
#include "definitions.h"

bool initSDL();
void render(const Chip8& chip8);
void cleanupSDL();
void handleInput(bool& running);
void updateDisplay(const Chip8& chip8);

#endif // GUI_H