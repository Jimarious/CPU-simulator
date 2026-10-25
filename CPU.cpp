#include "CPU.hpp"
#include "Instruction.hpp"
#include <iostream>
#include <fstream>
using namespace std;

int stall_count = 0;

int main() {
    bool end_flag = false;
    ifstream instruction_file("instructions", ios::binary);

    while (instruction_file.read(reinterpret_cast<char*>(&instruction_array[0]), sizeof(Instruction))) {
        instruction_array[4] = instruction_array[3];
        instruction_array[3] = instruction_array[2];
        // TD: Implement ID here and instruction_array[2,1, 0*] accordingly
        // TD: * stalls interfere with IF (may freeze it), however the while does not allow that or does it?
    return 0;
}

// TD: The just have a boolean stall, what if multiple stalls are needed?
int ID() {
    if (instruction_array[1].rs1_used) {
        for (int stage_idx=2; stage_idx<5; stage_idx++) {
            if (instruction_array[stage_idx].rd_used
            && instruction_array[1].rs1 == instruction_array[stage_idx].r) {
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