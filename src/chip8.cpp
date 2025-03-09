#include <iostream>
#include <fstream>
#include <array>
#include <string>
#include <iomanip>

#include "definitions.h"
#include "gui.h"

void read_file(const std::string filename, std::array<uint8_t, Chip8::MEMORY_SIZE>& ram) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Error: Failed to open file " << filename << std::endl;
        return;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    if (size > Chip8::MEMORY_SIZE - 0x200) {
        std::cerr << "Error: ROM too large to fit in memory!" << std::endl;
        return;
    }
    file.read(reinterpret_cast<char*>(&ram[0x200]), size);
    file.close();
}

uint16_t fetch(std::array<uint8_t, Chip8::MEMORY_SIZE>& ram, uint16_t& pc) {
    uint8_t high_byte = ram[pc];
    uint8_t low_byte = ram[pc + 1];
    uint16_t op = (high_byte << 8) | low_byte;

    pc += 0x2;
    return op;
}

void print_v_regs(const std::array<uint8_t, 16>& v_regs) {
    for (int i = 0; i < 16; ++i) {
        std::cout << "V" << i << ": 0x"
                  << std::setw(2) << std::setfill('0') << std::hex << (int)v_regs[i];
        if (i < 15) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;
}

int main() {
    Chip8 chip8;

    for (size_t i = 0; i < chip8.v_reg.size(); i++) {
        chip8.v_reg[i] = 0x0;
    }

    if (!initSDL()) {
        return 1;
    }

    std::cout << "DEBUG: Program Counter: 0x" << std::hex << chip8.pc <<std::endl;
    read_file("test.ch8", chip8.ram);

    std::cout << "DEBUG: RAM at 0x200: ";
    for (size_t i = 0; i < chip8.ram.size(); i++) {
        if (i == 0x200) {
            printf("\n0x200: %02X ", chip8.ram[i]);
        } else {
            printf("%02X ", chip8.ram[i]);
        }

    }
    std::cout << std::endl;

    bool running = true;
    while (running) {
        uint16_t op = fetch(chip8.ram, chip8.pc);
        chip8.exec(op);
        std::cout << "DEBUG: Current opcode " << std::hex << std::setw(4) << std::setfill('0') << op << std::endl;
        std::cout << "DEBUG: PC: " << std::hex << chip8.pc << std::endl;
        print_v_regs(chip8.v_reg);

        updateDisplay(chip8);
        handleInput(running);
    }
    cleanupSDL();
    /*while (chip8.pc <= 0xFFF) {
        uint16_t op = fetch(chip8.ram, chip8.pc);
        std::cout << "DEBUG: Current opcode " << std::hex << std::setw(4) << std::setfill('0') << op << std::endl;
        std::cout << "DEBUG: PC: " << std::hex << chip8.pc << std::endl;
    }*/


    return 0;
}