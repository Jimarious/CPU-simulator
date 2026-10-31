#include "Pipeline.hpp"
#include "Instruction.hpp"
#include <iostream>
#include <fstream>
#include <charconv>
#include <cstring>
#include <algorithm>

using namespace std;

int main(int argc, char *argv[]) {
    // TD: Make it read compressed files later
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
    bool end = false;

    ifstream instruction_file("instructions", ios::binary);

    while (1) {
        p.cycle();
    }
}

int check_depend(Instruction young, Instruction old) {
    return old.rd_used && ((young.rs1_used && young.rs1 == old.rd)
    || (young.rs2_used && young.rs2 == old.rd))
    || (young.cc_r && old.cc_w);
}


// TD: Add other instructions as well, currently considering only CBR, ALU and LD
int ID(bool &stall, bool FWD_EXE, bool FWD_MEM) {
    if (check_depend(instruction_array[1], instruction_array[2])
        && (!FWD_EXE || instruction_array[2].opcode == OP_LD))
        stall = true;
    else if (check_depend(instruction_array[1], instruction_array[3]) && !FWD_MEM)
        stall = true;
    else
        stall = false;
}

int EX() {
    // TD: Implement
}