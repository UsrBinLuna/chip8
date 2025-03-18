#include "definitions.h"

Chip8::Chip8()
    : pc(0x200), sp(0), i_reg(0), dt(0), st(0), waitingKey(0) {display.fill(false); keypad.fill(false);}
