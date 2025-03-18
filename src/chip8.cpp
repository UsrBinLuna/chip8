#include <iostream>
#include <fstream>
#include <array>
#include <string>
#include <iomanip>
#include <chrono>
#include <thread>
#include <atomic>

#include "definitions.h"
#include "gui.h"
#include "sound.h"

#include "../include/tinyfiledialogs.h"

std::atomic<bool> running(true);

// chip8 functions
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

void cycle(std::array<uint8_t, Chip8::MEMORY_SIZE>& ram, uint16_t& pc, Chip8& chip8) {
    uint16_t op = fetch(ram, pc);
    chip8.exec(op);
}


void timerLoop(Chip8& chip8, Sound &sound) {
    using namespace std::chrono;
    auto nextCycle = high_resolution_clock::now();
    while (running) {
        nextCycle += milliseconds(16);

        if (chip8.dt > 0) chip8.dt--;
        if (chip8.st > 0) {
            chip8.st--;
            sound.play(chip8.st > 0);
        }

        std::this_thread::sleep_until(nextCycle);
    }
}


int main(int argc, char** argv) {

    std::string filename;

    if (argc < 2) {
        const char* filters[] = {"*.ch8", "*.rom"};
        filename = tinyfd_openFileDialog("Select CHIP-8 ROM", "", 2, filters, "CHIP-8 ROM Files", 0);

        if (!filename.empty()) {
            std::cout << "Selected file: " << filename << std::endl;
        } else {
            std::cerr << "No file selected. Exiting.\n";
            return 1;
        }
    } else {
        filename = argv[1];
    }

    Chip8 chip8;
    Sound sound;

    std::thread timerThread(timerLoop, std::ref(chip8), std::ref(sound));

    for (size_t i = 0; i < chip8.v_reg.size(); i++) {
        chip8.v_reg[i] = 0x0;
    }

    if (!initSDL()) {
        return 1;
    }
    read_file(filename, chip8.ram);

    chip8.loadFonts(chip8, chip8.ram);

    for (size_t i = 0; i < chip8.ram.size(); i++) {
        if (i == 0x200) {
            printf("\n0x200: %02X ", chip8.ram[i]);
        } else {
            printf("%02X ", chip8.ram[i]);
        }
    }
    std::cout << std::endl;

    // clear screen cause some roms dont do that for some reason
    chip8.opcode_00E0();

    while (running.load()) {
        auto frameStart = std::chrono::high_resolution_clock::now();

        handleInput(running, chip8);

        if (!running) break;
        if (!paused) {
            for (int i = 0; i < Chip8::INSTRUCTIONS_PER_FRAME; i++) {
                cycle(chip8.ram, chip8.pc, chip8);
            }
        }
        if (chip8.displayChanged) {
            render(chip8);
            chip8.displayChanged = false;
        }
        if (chip8.debugUpdateCounter % 30 == 0) {
            renderDebugInfo(chip8);
        }
        chip8.debugUpdateCounter++;
        renderDebugWindow();
        auto frameTime = std::chrono::high_resolution_clock::now() - frameStart;
        auto remainingTime = std::chrono::milliseconds(Chip8::FRAME_DURATION_MS) - frameTime;
        if (remainingTime.count() > 0) {
            std::this_thread::sleep_for(remainingTime);
        }
    }
    cleanupSDL();

    return 0;
}