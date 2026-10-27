#include "CPU.hpp"
#include "Instruction.hpp"
#include <iostream>
#include <fstream>
#include <charconv>
#include <cstring>

using namespace std;

int main(int argc, char *argv[]) {
    // TD: Make it read compressed files later
    int PIPE_WIDTH = 1;
    int BR_PRED = 0;
    bool FWD_EXE = false;
    bool FWD_MEM = false;
    ifstream instruction_file;
    // TD: Only trace is essential make default handling for all other values and handle them
    for (int arg_idx = 1; arg_idx < argc; arg_idx++) {
        if (!std::strcmp(argv[arg_idx], "-pipewidth")) {
            if (++arg_idx == argc) {
                cerr << "Expected pipewidth value\n";
                return 1;
            }
            auto [ptr, err] = from_chars(argv[arg_idx], argv[arg_idx]+std::strlen(argv[arg_idx]), PIPE_WIDTH);
            if (err != std::errc{} || ptr != argv[arg_idx]+std::strlen(argv[arg_idx])
                || PIPE_WIDTH < 1 || PIPE_WIDTH > 8)  {
                cerr << "Invalid pipewidth value\n";
                return 1;
            }
        } else if (!std::strcmp(argv[arg_idx], "-bpredpolicy")) {
            if (++arg_idx == argc) {
                cerr << "Expected bpredpolicy value\n";
                return 1;
            }
            auto [ptr, err] = from_chars(argv[arg_idx], argv[arg_idx]+std::strlen(argv[arg_idx]), BR_PRED);
            if (ptr != argv[arg_idx]+strlen(argv[arg_idx]) || err != std::errc{}
                || BR_PRED < 0 || BR_PRED > 2) {
                    cerr << "Invalid bpredpolicy value\n";
                    return 1;
            }
        } else if (!std::strcmp(argv[arg_idx], "-enableexefwd")) {
            FWD_EXE = true;
        } else if (!std::strcmp(argv[arg_idx], "-enablememfwd")) {
            FWD_MEM = true;
        } else {
            instruction_file.open(argv[arg_idx], ios::binary);
            if(!instruction_file.is_open()) {
                cerr << "Invalid parameter: \"" << argv[arg_idx] << "\"\n";
                return 1;
            }
        }
    }
    if (!instruction_file.is_open() || instruction_file.fail()) {
        cerr << "Multiple or no trace files provided\n";
        return 1;
    }


    bool stall = false;

    ifstream instruction_file("instructions", ios::binary);

    while (instruction_file.read(reinterpret_cast<char*>(&instruction_array[0]), sizeof(Instruction))) {
        instruction_array[4] = instruction_array[3];
        instruction_array[3] = instruction_array[2];
    }

    instruction_file.close();
    return 0;
}

// TD: I considere forwarding disabled, make a enable flag and handling
// TD: If this is not called many times do not make it a funciton
int check_rw_depend(Instruction young, Instruction old) {
    return old.rd_used && ((young.rs1_used && young.rs1 == old.rd)
    || (young.rs2_used && young.rs2 == old.rd));
}

// TD: Make cases for FWD_EXE and FWD_MEM
// TD: What about freezes
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