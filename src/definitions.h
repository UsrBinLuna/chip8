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

    void read_file(const std::string filename, std::array<uint8_t, Chip8::MEMORY_SIZE>& ram);
    void exec(uint16_t op);
    std::pair<uint8_t, bool> wrapping_add(uint8_t a, uint8_t b);
    void push(uint16_t val);
    uint16_t pop();

    std::vector<uint8_t> rom;
    std::array<uint8_t, 4096> ram{};

    // opcodes
    void opcode_00E0();
    void opcode_1NNN(uint16_t& op);
    void opcode_6XNN(uint16_t& op);
    void opcode_7XNN(uint16_t& op);
    void opcode_ANNN(uint16_t& op);
    void opcode_3XNN(uint16_t& op);
    void opcode_4XNN(uint16_t& op);
    void opcode_5XY0(uint16_t& op);
    void opcode_9XY0(uint16_t& op);
    void opcode_00EE();
    void opcode_2NNN(uint16_t& op);
    void opcode_8XY0(uint16_t& op);
    void opcode_8XY1(uint16_t& op);
    void opcode_8XY2(uint16_t& op);
    void opcode_8XY3(uint16_t& op);
    void opcode_8XY4(uint16_t& op);
    void opcode_8XY5(uint16_t& op);
    void opcode_8XY6(uint16_t& op);
    void opcode_8XY7(uint16_t& op);
    void opcode_8XYE(uint16_t& op);
    void opcode_DXYN(uint16_t& op);
    void opcode_FX1E(uint16_t& op);
    void opcode_FX33(uint16_t& op);
    void opcode_FX55(uint16_t& op);
    void opcode_FX65(uint16_t& op);

};

#endif // DEFINITIONS_H
