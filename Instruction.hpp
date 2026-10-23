#pragma once
#include <string>
#include <unordered_map>

enum class Opcode {
    NOP,
    ADD,
    LW,
    SW,
    BEQ
};

struct opcode_details {
    int in_stage;
    int out_stage;
    int n_src_regs;
    int n_dest_regs;
};

// Convert instruction strings to Opcode values
const std::unordered_map<std::string, Opcode> str_to_opcode{
    {"NOP", Opcode::NOP},
    {"ADD", Opcode::ADD},
    {"LW",  Opcode::LW},
    {"SW",  Opcode::SW},
    {"BEQ", Opcode::BEQ}
};

// Store information about each instruction type
const std::unordered_map<Opcode, opcode_details> opcode_dict{
    {Opcode::NOP, {0, 0, 0, 0}},
    {Opcode::ADD, {2, 3, 2, 1}},
    {Opcode::LW,  {2, 4, 1, 1}},
    {Opcode::SW,  {2, 0, 2, 0}},
    {Opcode::BEQ, {2, 0, 2, 0}}
};

class Instruction {
public:
    Opcode opcode = Opcode::NOP;
    int reg[3] = {-1, -1, -1}; // reg[0] is destination
};