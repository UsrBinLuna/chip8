#include "definitions.h"
#include <iostream>
#include <iomanip>
#include <array>
#include <sstream>
#include <random>

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
    clearScreen();
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

void Chip8::opcode_BNNN(uint16_t& op) {
    uint16_t addr = op & 0x0FFF;
    pc = addr + v_reg[0];
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
    // cosmac vip quirk, vf is reset with 8XY1-3
    v_reg[0xF] = 0x0;
}

void Chip8::opcode_8XY2(uint16_t& op) {
    uint8_t x = (op & 0x0F00) >> 8;
    uint8_t y = (op & 0x00F0) >> 4;
    v_reg[x] = v_reg[x] & v_reg[y];
    // cosmac vip quirk, vf is reset with 8XY1-3
    v_reg[0xF] = 0x0;
}

void Chip8::opcode_8XY3(uint16_t& op) {
    uint8_t x = (op & 0x0F00) >> 8;
    uint8_t y = (op & 0x00F0) >> 4;
    v_reg[x] = v_reg[x] ^ v_reg[y];
    // cosmac vip quirk, vf is reset with 8XY1-3
    v_reg[0xF] = 0x0;
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
    uint8_t bit = v_reg[x];
    v_reg[x] -= v_reg[y];
    if (bit >= v_reg[y]) v_reg[0xF] = 1; else v_reg[0xF] = 0;
}

void Chip8::opcode_8XY6(uint16_t& op) {
    uint8_t y = (op & 0x00F0) >> 4;
    uint8_t x = (op & 0x0F00) >> 8;
    v_reg[x] = v_reg[y];
    uint8_t bit = v_reg[x] & 0x1;
    v_reg[x] >>= 1;
    v_reg[0xF] = bit;
}

void Chip8::opcode_8XY7(uint16_t& op) {
    uint8_t y = (op & 0x00F0) >> 4;
    uint8_t x = (op & 0x0F00) >> 8;
    uint8_t temp_x = v_reg[x];
    v_reg[x] = v_reg[y] - v_reg[x];
    if (v_reg[y] >= temp_x) v_reg[0xF] = 1; else v_reg[0xF] = 0;
}

void Chip8::opcode_8XYE(uint16_t& op) {
    uint8_t y = (op & 0x00F0) >> 4;
    uint8_t x = (op & 0x0F00) >> 8;
    v_reg[x] = v_reg[y];
    uint8_t bit = (v_reg[x] & 0x80) >> 7;
    v_reg[x] <<= 1;
    v_reg[0xF] = bit;

}

void Chip8::opcode_CXNN(uint16_t& op) {
    uint8_t x = (op & 0x0F00) >> 8;
    uint16_t val = op & 0x00FF;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 255);
    uint8_t rand = dist(gen) & val;
    v_reg[x] = rand;
}

// timers
void Chip8::opcode_FX07(uint16_t& op) {
    uint8_t x = (op & 0x0F00) >> 8;
    v_reg[x] = dt;
}

void Chip8::opcode_FX15(uint16_t& op) {
    uint8_t x = (op & 0x0F00) >> 8;
    dt = v_reg[x];
}

void Chip8::opcode_FX18(uint16_t& op) {
    uint8_t x = (op & 0x0F00) >> 8;
    st = v_reg[x];
}

void Chip8::opcode_FX1E(uint16_t &op) {
    uint8_t x = (op & 0x0F00) >> 8;
    i_reg += v_reg[x];
    if (i_reg >= 0x1000) {
        v_reg[0xF] = 1;
    }
}

// keypad!!!!!!!!!!!
void Chip8::opcode_FX0A(uint16_t& op) {
    uint8_t x = (op & 0x0F00) >> 8;
    if (!waitingForKey) {
        for (uint8_t i = 0; i < 16; i++) {
            if (keypad[i]) {
                v_reg[x] = i;
                waitingForKey = true;
                waitingKey = i;
                pc -= 2;
                break;
            }
        }
        pc -= 0x2;
    } else {
        if (!keypad[waitingKey]) {
            waitingForKey = false;
        } else {
            pc -= 2;
        }
    }
}

void Chip8::opcode_EX9E(uint16_t& op) {
    uint8_t x = (op & 0x0F00) >> 8;
    if (keypad[v_reg[x]]) {
        pc += 2;
    }
}

void Chip8::opcode_EXA1(uint16_t& op) {
    uint8_t x = (op & 0x0F00) >> 8;
    if (!keypad[v_reg[x]]) {
        pc += 2;
    }
}

void Chip8::opcode_FX55(uint16_t& op) {
    uint16_t addr = i_reg;
    uint8_t x = (op & 0x0F00) >> 8;
    for (uint8_t i = 0; i <= x; i++) {
        ram[addr] = v_reg[i];
        addr += 0x1;
        // cosmac vip quirk, index increases too
        i_reg++;
    }
}

void Chip8::opcode_FX65(uint16_t& op) {
    uint16_t addr = i_reg;
    uint8_t x = (op & 0x0F00) >> 8;
    for (uint8_t i = 0; i <= x; i++) {
        v_reg[i] = ram[addr];
        addr += 0x1;
        // cosmac vip quirk, index increases too
        i_reg++;
    }
}

void Chip8::opcode_FX33(uint16_t& op) {
    uint8_t x = (op & 0x0F00) >> 8;
    uint8_t val = v_reg[x];

    std::array<uint8_t, 3> number = {};
    number[0]= val / 100;
    number[1] = (val / 10) % 10;
    number[2] = val % 10;

    for (uint8_t i = 0; i <= 2; i++) {
        ram[i_reg + i] = number[i];
    }
}


// drawing stuff
void Chip8::opcode_DXYN(uint16_t& op) {
    uint8_t x_reg = (op & 0x0F00) >> 8;
    uint8_t y_reg = (op & 0x00F0) >> 4;
    uint8_t height = op & 0x000F;
    v_reg[0xF] = 0;
    uint8_t x_coord = v_reg[x_reg] & 63;
    uint8_t y_coord = v_reg[y_reg] & 31;
    v_reg[0xF] = 0;

    for (uint8_t row = 0; row < height; row++) {
        uint8_t sprite_row = ram[i_reg + row];
        for (uint8_t col = 0; col < 8; col++) {
            if (sprite_row & (0x80 >> col)) {
                int pixelX = (x_coord + col) % Chip8::DISPLAY_WIDTH;
                int pixelY = (y_coord + row) % Chip8::DISPLAY_HEIGHT;
                bool prevPixel = display[pixelY * Chip8::DISPLAY_WIDTH + pixelX];
                if (prevPixel) v_reg[0xF] = 1;
                setPixel(pixelX, pixelY, !prevPixel);
            }
        }
    }
    displayChanged = true;
}

// font character
void Chip8::opcode_FX29(uint16_t& op) {
    uint8_t x = (op & 0x0F00) >> 8;
    uint8_t character = v_reg[x] & 0x0F;
    switch (character) {
        case 0x0: i_reg = 0x50; break;
        case 0x1: i_reg = 0x55; break;
        case 0x2: i_reg = 0x5A; break;
        case 0x3: i_reg = 0x5F; break;
        case 0x4: i_reg = 0x64; break;
        case 0x5: i_reg = 0x69; break;
        case 0x6: i_reg = 0x6E; break;
        case 0x7: i_reg = 0x73; break;
        case 0x8: i_reg = 0x78; break;
        case 0x9: i_reg = 0x7D; break;
        case 0xA: i_reg = 0x82; break;
        case 0xB: i_reg = 0x87; break;
        case 0xC: i_reg = 0x8C; break;
        case 0xD: i_reg = 0x91; break;
        case 0xE: i_reg = 0x96; break;
        case 0xF: i_reg = 0x9B; break;
        default: i_reg = 0x50; break;
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
        case 0xB: opcode_BNNN(op); break;
        case 0xC: opcode_CXNN(op); break;
        case 0xD: opcode_DXYN(op); break;
        case 0xE:
            if (d4 == 1) { opcode_EXA1(op); }
            if (d4 == 0xE) { opcode_EX9E(op); }
        break;
        case 0xF:
            switch (op & 0x00FF) {
                case 0x07: opcode_FX07(op); break;
                case 0x15: opcode_FX15(op); break;
                case 0x18: opcode_FX18(op); break;
                case 0x1E: opcode_FX1E(op); break;
                case 0x29: opcode_FX29(op); break;
                case 0X0A: opcode_FX0A(op); break;
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
