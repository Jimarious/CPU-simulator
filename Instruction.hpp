#pragma once
#include <string>
#include <unordered_map>
#include <cstdint>

struct Instruction {
    uint64_t instruction_address = 0;
    uint8_t opcode = 0;
    uint8_t rd = 0;
    uint8_t rd_used = 0;
    uint8_t rs1 = 0, rs2 = 0;
    uint8_t rs1_used = 0, rs2_used = 0;
    uint16_t condition_flags = 0; // TD: Not know if I defined this right
    uint64_t mem_addr = 0;
    uint8_t mem_w = 0, mem_r = 0, br_dir = 0;
    uint64_t br_target = 0;
};

/*
const std::unordered_map<std::string, Instruction> instruction_map{
    {"NOP", {Opcode::NOP, 0, 0, 0, 0}},
    {"ADD", {Opcode::ADD, 2, 3, 2, 1}},
    {"LW",  {Opcode::LW,  2, 4, 1, 1}},
    {"SW",  {Opcode::SW,  2, 0, 2, 0}},
    {"BEQ", {Opcode::BEQ, 2, 0, 2, 0}}
};
*/