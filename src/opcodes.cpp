#include "definitions.h"
#include <iostream>
#include <iomanip>
#include <array>
#include <limits>

uint8_t Chip8::wrapping_add(uint8_t a, uint8_t b) {
    return static_cast<uint8_t>((a + b) & std::numeric_limits<uint8_t>::max());
}

void Chip8::opcode_00E0() {
    std::cout << "Clear screen!" << std::endl;
    std::fill(display.begin(), display.end(), false);
}

void Chip8::opcode_1NNN(uint16_t& op) {
    uint16_t addr = op & 0x0FFF;
    pc = addr;
}

void Chip8::opcode_6XNN(uint16_t& op) {
    uint16_t val = op & 0x00FF;
    uint16_t reg = (op & 0x0F00) >> 8;
    v_reg[reg] = val;
}

void Chip8::opcode_7XNN(uint16_t& op) {
    uint16_t val = op & 0x00FF;
    uint16_t reg = (op & 0x0F00) >> 8;
    v_reg[reg] = wrapping_add(v_reg[reg], val);
    std::cout << v_reg[reg] << std::endl;
}

void Chip8::opcode_ANNN(uint16_t& op) {
    uint16_t val = op & 0x0FFF;
    i_reg = val;
}

// 8___ instructions
void Chip8::opcode_8XY0(uint16_t& op) {
    uint8_t x = (op & 0x0F00) >> 8;
    uint8_t y = (op & 0x00F0) >> 4;
    v_reg[x] = v_reg[y];
}

void Chip8::opcode_8XY1(uint16_t& op) {
    uint8_t x = (op & 0x0F00) >> 8;
    uint8_t y = (op & 0x00F0) >> 4;
    v_reg[x] = v_reg[x] | v_reg[y];
}

void Chip8::opcode_8XY2(uint16_t& op) {
    uint8_t x = (op & 0x0F00) >> 8;
    uint8_t y = (op & 0x00F0) >> 4;
    v_reg[x] = v_reg[x] & v_reg[y];
}

void Chip8::opcode_DXYN(uint16_t& op) {
    uint8_t x_reg = (op & 0x0F00) >> 8;
    uint8_t y_reg = (op & 0x00F0) >> 4;
    uint8_t height = op & 0x000F;

    uint8_t x_coord = v_reg[x_reg] & 63;
    uint8_t y_coord = v_reg[y_reg] & 31;
    v_reg[0xF] = 0;

    for (uint8_t row = 0; row <= height; row++) {
        uint8_t sprite_row = ram[i_reg + row];

        for (uint8_t col = 0; col < 8; col++) {
            if (sprite_row & (0x80 >> col)) {
                uint16_t display_index = (y_coord + row) * Chip8::DISPLAY_WIDTH + (x_coord + col);
                if (display[display_index]) {
                    v_reg[0xF] = 1;
                }
                if (display_index < 2048) {
                    if (display[display_index]) {
                        v_reg[0xF] = 1;
                    }
                    display[display_index] ^= 1;
                }
            }
        }

    }

}


void Chip8::exec(uint16_t op) {

    uint16_t d1 = (op & 0xF000) >> 12;
    uint16_t d2 = (op & 0x0F00) >> 8;
    uint16_t d3 = (op & 0x00F0) >> 4;
    uint16_t d4 = op & 0x000F;

    switch (d1) {
        case 0x0:
            if (op == 0x00E0) { opcode_00E0(); }
        break;
        case 0x1: opcode_1NNN(op); break;
        case 0x6: opcode_6XNN(op); break;
        case 0x7: opcode_7XNN(op); break;
        case 0xA: opcode_ANNN(op); break;
        case 0xD: opcode_DXYN(op); break;
        default:
            std::cout << "Unknown opcode: " << std::hex << op << std::endl;
        break;
    }
}
