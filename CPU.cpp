#include "CPU.hpp"
#include "Instruction.hpp"
#include <iostream>
#include <fstream>
using namespace std;

int stall_count = 0;

int main() {
    bool end_flag = false;
    string instruction;
    ifstream instruction_file("instructions", ios::binary);

    while (instruction_file.read(reinterpret_cast<char*>(instruction), sizeof(Instruction))) {
    // Make IF later, now here because of file
        WB();
        MEM();
        EX();
        instruction_array[4] = instruction_array[3];
        instruction_array[3] = instruction_array[2];
        instruction_array[2] = instruction_array[1];
        ID();
        // ************* IF start ****************
        // TD: Forgot to parse regs
        if (instruction_file >> opcode) {
            instruction_array[0] = instruction_map.at(opcode);
            for (int in_reg_idx = 0; in_reg_idx < instruction_array[0].n_src_regs; in_reg_idx++) {
                instruction_array[0].reg[in_reg_idx]
            }
        } else {
            end_flag = 1; // TD: find a way to end
        }
        // ************* IF end ******************
    }

    return 0;
}

int ID() {
    for (int id_in_idx=1; id_in_idx<2; id_in_idx++) {
        if (instruction_array[1].reg[id_in_idx] != -1) {
            for (int stage_idx=2; stage_idx<5; stage_idx++) {
                if (instruction_array[stage_idx].reg[0] != 1
                && instruction_array[1].reg[id_in_idx] == instruction_array[stage_idx].reg[0]) {
                    stall_count = max(opcode_dict[instruction_array[stage_idx].opcode].out_stage
                        - stage_idx - opcode_dict[instruction_array[1].opcode].in_stage + 1, stall_count);
            }
        }
    }
}

int EX() {
    // TD: Implement
}

/*
ALU    R2,R3    R1      -            -
LD     R5       R4      LD@0x1000    -
ALU    R4,R1    R6      -            -
ST     R6       -       ST@0x2000    -
ALU    R1,R2    R7      -            -
CBR    -        -       -            T->0x400028
ALU    R6,R8    R9      -            -
OTHER  -        -       -            -
*/