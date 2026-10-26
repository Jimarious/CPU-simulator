#pragma once
#include <string>
#include <unordered_map>
#include <cstdint>

typedef enum {
    OP_ALU,
    OP_LD,
    kristo, // TD: Update, Just taking up 2
    OP_CBR
} Opcode;

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