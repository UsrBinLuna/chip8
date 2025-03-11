#include "definitions.h"
#include <iostream>
#include <iomanip>
#include <array>
#include <sstream>
#include <string>
#include <limits>

// useful functions
std::pair<uint8_t, bool> Chip8::wrapping_add(uint8_t a, uint8_t b) {
    uint16_t sum = a + b;
    return { static_cast<uint8_t>(sum & 0xFF), sum > 0xFF };
}


void Chip8::push(uint16_t val) {
    stack[sp] = val;
    sp++;
}

uint16_t Chip8::pop() {
    if (sp > 0) {
        sp -= 1;
        return stack[sp];
    } else {
        return 1;
    }
}

// opcodes
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
    auto [result, overflow] = wrapping_add(v_reg[reg], val);
    v_reg[reg] = result;
    std::cout << v_reg[reg] << std::endl;
}

void Chip8::opcode_ANNN(uint16_t& op) {
    uint16_t val = op & 0x0FFF;
    i_reg = val;
}

// skip instructions
void Chip8::opcode_3XNN(uint16_t& op) {
    uint16_t val = op & 0x00FF;
    uint16_t reg = (op & 0x0F00) >> 8;
    if (v_reg[reg] == val) {
        pc += 0x2;
    }
}

void Chip8::opcode_4XNN(uint16_t& op) {
    uint16_t val = op & 0x00FF;
    uint16_t reg = (op & 0x0F00) >> 8;
    if (v_reg[reg] != val) {
        pc += 0x2;
    }
}

void Chip8::opcode_5XY0(uint16_t& op) {
    uint8_t x = (op & 0x0F00) >> 8;
    uint8_t y = (op & 0x00F0) >> 4;
    if (v_reg[x] == v_reg[y]) {
        pc += 0x2;
    }
}

void Chip8::opcode_9XY0(uint16_t& op) {
    uint8_t x = (op & 0x0F00) >> 8;
    uint8_t y = (op & 0x00F0) >> 4;
    if (v_reg[x] != v_reg[y]) {
        pc += 0x2;
    }
}

// subroutines
void Chip8::opcode_00EE() {
    pc = pop();
}

void Chip8::opcode_2NNN(uint16_t& op) {
    uint16_t addr = op & 0x0FFF;
    push(pc);
    pc = addr;
}

// 8___ instructions
void Chip8::opcode_8XY0(uint16_t& op) {
    uint8_t x = (op & 0x0F00) >> 8;
    uint8_t y = (op & 0x00F0) >> 4;
    v_reg[x] = v_reg[y];

    std::cout << "DEBUG: 8XY0 - Set V" << std::dec << (int)x
              << " = V" << (int)y << " (" << std::hex << (int)v_reg[y] << ")"
              << std::endl;
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

void Chip8::opcode_8XY3(uint16_t& op) {
    uint8_t x = (op & 0x0F00) >> 8;
    uint8_t y = (op & 0x00F0) >> 4;
    v_reg[x] = v_reg[x] ^ v_reg[y];
}

void Chip8::opcode_8XY4(uint16_t& op) {
    uint8_t y = (op & 0x00F0) >> 4;
    uint8_t x = (op & 0x0F00) >> 8;
    auto [result, overflow] = wrapping_add(v_reg[x], v_reg[y]);
    v_reg[x] = result;
    if (overflow) {
        v_reg[0xF] = 0x1;
    } else {
        v_reg[0xF] = 0x0;
    }
}

void Chip8::opcode_8XY5(uint16_t& op) {
    uint8_t y = (op & 0x00F0) >> 4;
    uint8_t x = (op & 0x0F00) >> 8;
    if (v_reg[x] > v_reg[y]) v_reg[0xF] = 1; else v_reg[0xF] = 0;
    v_reg[x] -= v_reg[y];
}

void Chip8::opcode_8XY6(uint16_t& op) {
    uint8_t y = (op & 0x00F0) >> 4;
    uint8_t x = (op & 0x0F00) >> 8;
    v_reg[x] = v_reg[y];
    v_reg[x] = v_reg[x] >> 0x1;
}

void Chip8::opcode_8XY7(uint16_t& op) {
    uint8_t y = (op & 0x00F0) >> 4;
    uint8_t x = (op & 0x0F00) >> 8;
    if (v_reg[y] > v_reg[x]) v_reg[0xF] = 1; else v_reg[0xF] = 0;
    v_reg[x] = v_reg[y] - v_reg[x];
}

void Chip8::opcode_8XYE(uint16_t& op) {
    uint8_t y = (op & 0x00F0) >> 4;
    uint8_t x = (op & 0x0F00) >> 8;
    v_reg[x] = v_reg[y];
    v_reg[x] = v_reg[x] << 0x1;
}

void Chip8::opcode_FX1E(uint16_t &op) {
    uint8_t x = (op & 0x0F00) >> 8;
    i_reg += v_reg[x];
    if (i_reg >= 0x1000) {
        v_reg[0xF] = 1;
    }
}

// todo: fix
void Chip8::opcode_FX55(uint16_t& op) {
    uint16_t addr = i_reg;
    uint8_t x = (op & 0x0F00) >> 8;
    for (uint8_t i = 0; i <= x; i++) {
        ram[addr] = v_reg[i];
        addr += 0x2;
    }
}

// todo: fix
void Chip8::opcode_FX65(uint16_t& op) {
    uint16_t addr = i_reg;
    uint8_t x = (op & 0x0F00) >> 8;
    for (uint8_t i = 0; i <= x; i++) {
        v_reg[i] = ram[addr];
        std::cout << "Loading from address 0x" << std::hex << addr
          << " to V" << std::dec << (int)i
          << " value: 0x" << std::hex << (int)ram[addr]
          << std::dec << std::endl;
        addr += 0x2;
    }
}

void Chip8::opcode_FX33(uint16_t& op) {
    uint8_t x = (op & 0x0F00) >> 8;
    uint8_t val = v_reg[x];
    std::cout << "\nI reg: " << std::hex << (int)i_reg << std::endl;

    std::array<uint8_t, 3> number = {};
    number[0]= val / 100;
    number[1] = (val / 10) % 10;
    number[2] = val % 10;
    std::cout << "Digits: " << (int)number[0] << " " << (int)number[1] << " " << (int)number[2] << std::endl;

    for (uint8_t i = 0; i <= 2; i++) {
        ram[i_reg + i] = number[i];
        std::cout << "Storing " << (int)number[i] << " at 0x" << std::hex << (i_reg + i) << std::endl;
        std::cout << "Memory address: " << std::hex << (int)ram[i_reg + i] << std::dec << std::endl;
    }
}

void Chip8::opcode_DXYN(uint16_t& op) {
    uint8_t x_reg = (op & 0x0F00) >> 8;
    uint8_t y_reg = (op & 0x00F0) >> 4;
    uint8_t height = op & 0x000F;

    uint8_t x_coord = v_reg[x_reg] & 63;
    uint8_t y_coord = v_reg[y_reg] & 31;
    v_reg[0xF] = 0;

    for (uint8_t row = 0; row < height; row++) {
        uint8_t sprite_row = ram[i_reg + row];

        for (uint8_t col = 0; col < 8; col++) {
            if (sprite_row & (0x80 >> col)) {
                uint16_t display_index = (y_coord + row) * Chip8::DISPLAY_WIDTH + (x_coord + col);
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
            if (op == 0x00EE) { opcode_00EE(); }
        break;
        case 0x1: opcode_1NNN(op); break;
        case 0x2: opcode_2NNN(op); break;
        case 0x3: opcode_3XNN(op); break;
        case 0x4: opcode_4XNN(op); break;
        case 0x5: opcode_5XY0(op); break;
        case 0x6: opcode_6XNN(op); break;
        case 0x7: opcode_7XNN(op); break;
        case 0x8:
            if (d4 == 0) { opcode_8XY0(op); }
            if (d4 == 1) { opcode_8XY1(op); }
            if (d4 == 2) { opcode_8XY2(op); }
            if (d4 == 3) { opcode_8XY3(op); }
            if (d4 == 4) { opcode_8XY4(op); }
            if (d4 == 5) { opcode_8XY5(op); }
            if (d4 == 6) { opcode_8XY6(op); }
            if (d4 == 7) { opcode_8XY7(op); }
            if (d4 == 0xE) { opcode_8XYE(op); }
        break;
        case 0x9: opcode_9XY0(op); break;
        case 0xA: opcode_ANNN(op); break;
        case 0xD: opcode_DXYN(op); break;
        case 0xF:
            switch (op & 0x00FF) {
                case 0xFF: opcode_FX1E(op); break;
                case 0x65: opcode_FX65(op); break;
                case 0x55: opcode_FX55(op); break;
                case 0x33: opcode_FX33(op); break;
                default: std::cout << "Unknown FX opcode: " << std::hex << op << std::endl; break;
            }
        break;
        default:
            std::cout << "Unknown opcode: " << std::hex << op << std::endl;
        break;
    }
}
