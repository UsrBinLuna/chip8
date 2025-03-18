#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <cstdint>
#include <vector>
#include <string>
#include <array>
#include <SDL2/SDL_rect.h>

class Chip8 {
public:
    Chip8();

    static constexpr size_t MEMORY_SIZE = 4096;
    static constexpr size_t DISPLAY_WIDTH = 64;
    static constexpr size_t DISPLAY_HEIGHT = 32;
    static constexpr size_t REGS = 16;
    static constexpr size_t KEYS = 16;
    static constexpr size_t STACK_SIZE = 16;
    static constexpr int INSTRUCTIONS_PER_FRAME = 10;
    static constexpr int FRAME_DURATION_MS = 1000 / 60;

    int debugUpdateCounter = 0;
    mutable std::vector<SDL_Rect> dirtyRects;
    mutable bool displayChanged = false;

    std::array<bool, DISPLAY_WIDTH * DISPLAY_HEIGHT> display;

    void setPixel(int x, int y, bool value) {
        if (x < 0 || x >= DISPLAY_WIDTH || y < 0 || y >= DISPLAY_HEIGHT) return;

        int index = y * DISPLAY_WIDTH + x;
        if (display[index] != value) {
            display[index] = value;
            dirtyRects.push_back({x, y, 1, 1});
            displayChanged = true;
        }
    }

    void clearScreen() {
        display.fill(false);
        dirtyRects.clear();
        dirtyRects.push_back({0, 0, Chip8::DISPLAY_WIDTH, Chip8::DISPLAY_HEIGHT});
        displayChanged = true;
    }

    uint16_t pc;
    uint16_t sp;
    uint16_t i_reg;
    uint8_t dt;
    uint8_t st;
    bool waitingForKey = false;
    uint8_t waitingKey;
    std::array<uint8_t, REGS> v_reg;
    std::array<bool, KEYS> keypad;
    std::array<uint16_t, STACK_SIZE> stack;

    void read_file(const std::string filename, std::array<uint8_t, Chip8::MEMORY_SIZE>& ram);
    void exec(uint16_t op);
    uint16_t fetch(std::array<uint8_t, Chip8::MEMORY_SIZE>& ram, uint16_t& pc);
    void cycle(uint16_t op);
    void loadFonts(Chip8& chip8, std::array<uint8_t, Chip8::MEMORY_SIZE>& ram);
    std::pair<uint8_t, bool> wrapping_add(uint8_t a, uint8_t b);
    void push(uint16_t val);
    void updateDisplay();
    uint16_t pop();

    std::vector<uint8_t> rom;
    std::array<uint8_t, 4096> ram{};

    // fonts yay!!!
    std::array<uint8_t, 80> font{};

    // opcodes
    void opcode_00E0();
    void opcode_1NNN(uint16_t& op);
    void opcode_6XNN(uint16_t& op);
    void opcode_7XNN(uint16_t& op);
    void opcode_ANNN(uint16_t& op);
    void opcode_BNNN(uint16_t& op);
    void opcode_3XNN(uint16_t& op);
    void opcode_4XNN(uint16_t& op);
    void opcode_5XY0(uint16_t& op);
    void opcode_9XY0(uint16_t& op);
    void opcode_00EE();
    void opcode_2NNN(uint16_t& op);
    // arithmetic operations
    void opcode_8XY0(uint16_t& op);
    void opcode_8XY1(uint16_t& op);
    void opcode_8XY2(uint16_t& op);
    void opcode_8XY3(uint16_t& op);
    void opcode_8XY4(uint16_t& op);
    void opcode_8XY5(uint16_t& op);
    void opcode_8XY6(uint16_t& op);
    void opcode_8XY7(uint16_t& op);
    void opcode_8XYE(uint16_t& op);
    // draw
    void opcode_DXYN(uint16_t& op);
    void opcode_FX29(uint16_t& op);
    void opcode_CXNN(uint16_t& op);
    void opcode_FX07(uint16_t& op);
    // timers
    void opcode_FX15(uint16_t& op);
    void opcode_FX18(uint16_t& op);
    void opcode_FX1E(uint16_t& op);
    // keypad
    void opcode_FX0A(uint16_t& op);
    void opcode_EXA1(uint16_t& op);
    void opcode_EX9E(uint16_t& op);
    // ram stuff
    void opcode_FX33(uint16_t& op);
    void opcode_FX55(uint16_t& op);
    void opcode_FX65(uint16_t& op);

};

#endif // DEFINITIONS_H
