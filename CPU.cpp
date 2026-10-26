#include "CPU.hpp"
#include "Instruction.hpp"
#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char *argv[]) {
    // TD: Only trace is essential make default handling for all other values and handle them
    int PIPE_WIDTH;
    bool FWD_EXE;
    bool FWD_MEM;
    int BR_PRED;

    bool stall = false;

    ifstream instruction_file("instructions", ios::binary);

    while (instruction_file.read(reinterpret_cast<char*>(&instruction_array[0]), sizeof(Instruction))) {
        instruction_array[4] = instruction_array[3];
        instruction_array[3] = instruction_array[2];
        // TD: Implement ID here and instruction_array[2,1, 0*] accordingly
        // TD: * stalls interfere with IF (may freeze it), however the while does not allow that or does it?
    }
    return 0;
}

// ############ TD: Unordered map introduces delay, do it straight from opcode (Just do ALU, LD and CBR with forwarding enabled)
// TD: I considere forwarding disabled, make a enable flag and handling
// TD: If this is not called many times do not make it a funciton
int check_rw_depend(Instruction young, Instruction old) {
    return old.rd_used && ((young.rs1_used && young.rs1 == old.rd)
    || (young.rs2_used && young.rs2 == old.rd));
}

// TD: Make cases for FWD_EXE and FWD_MEM
int ID(bool &stall, bool FWD_EXE, bool FWD_MEM) {
    for (int old_idx=2; old_idx<4; old_idx++) {
        if (check_rw_depend(instruction_array[1], instruction_array[old_idx])) {
            if (FWD_FLAG) {
                switch (instruction_array[old_idx].opcode) {
                    case OP_ALU:
                        stall 
                }
            } else {
                stall = true;
            }
        }
    }
}

int EX() {
    // TD: Implement
}