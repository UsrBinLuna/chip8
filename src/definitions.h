#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <cstdint>
#include <vector>
#include <string>
#include <array>

class Chip8 {
public:
    Chip8();

    static constexpr size_t MEMORY_SIZE = 4096;
    static constexpr size_t DISPLAY_WIDTH = 64;
    static constexpr size_t DISPLAY_HEIGHT = 32;
    static constexpr size_t REGS = 16;
    static constexpr size_t STACK_SIZE = 16;

    uint16_t pc;
    uint16_t sp;
    uint16_t i_reg;
    uint8_t dt;
    uint8_t st;
    std::array<uint8_t, REGS> v_reg;
    std::array<bool, DISPLAY_WIDTH * DISPLAY_HEIGHT> display;
    std::array<uint16_t, STACK_SIZE> stack;

    void readFile(const std::string filename, std::array<uint8_t, Chip8::MEMORY_SIZE>& ram);
    void exec(uint16_t op);
    uint8_t wrapping_add(uint8_t a, uint8_t b);

    std::vector<uint8_t> rom;
    std::array<uint8_t, 4096> ram{};

    // opcodes
    void opcode_00E0();
    void opcode_1NNN(uint16_t& op);
    void opcode_6XNN(uint16_t& op);
    void opcode_7XNN(uint16_t& op);
    void opcode_ANNN(uint16_t& op);
    void opcode_8XY0(uint16_t& op);
    void opcode_8XY1(uint16_t& op);
    void opcode_8XY2(uint16_t& op);
    void opcode_DXYN(uint16_t& op);

};

#endif // DEFINITIONS_H
