#include "instruction.hpp"
#include "pipeline.hpp"
#include <iostream>
#include <fstream>
#include <charconv>
#include <cstring>
#include <algorithm>

using namespace std;

int check_depend(Instruction young, Instruction old) {
    return old.rd_used && ((young.rs1_used && young.rs1 == old.rd)
    || (young.rs2_used && young.rs2 == old.rd))
    || (young.cc_r && old.cc_w);
}


Pipeline::Pipeline(int argc, char *argv[]) {
    // TD: Make it read compressed files later
    for (int arg_idx = 1; arg_idx < argc; arg_idx++) {
        if (!strcmp(argv[arg_idx], "-pipewidth")) {
            if (++arg_idx == argc) {
                throw runtime_error("Expected pipewidth value\n");
            }
            auto [ptr, err] = from_chars(argv[arg_idx], argv[arg_idx]+strlen(argv[arg_idx]), PIPE_WIDTH);
            if (err != errc{} || ptr != argv[arg_idx]+strlen(argv[arg_idx])
                || PIPE_WIDTH < 1 || PIPE_WIDTH > 8)  {
                throw runtime_error("Invalid pipewidth value\n");
            }
        } else if (!strcmp(argv[arg_idx], "-bpredpolicy")) {
            if (++arg_idx == argc) {
                throw runtime_error("Expected bpredpolicy value\n");
            }
            auto [ptr, err] = from_chars(argv[arg_idx], argv[arg_idx]+strlen(argv[arg_idx]), BR_PRED);
            if (ptr != argv[arg_idx]+strlen(argv[arg_idx]) || err != errc{}
                || BR_PRED < 0 || BR_PRED > 2) {
                    throw runtime_error("Invalid bpredpolicy value\n");
            }
        } else if (!strcmp(argv[arg_idx], "-enableexefwd")) {
            FWD_EXE = true;
        } else if (!strcmp(argv[arg_idx], "-enablememfwd")) {
            FWD_MEM = true;
        } else {
            instruction_file.open(argv[arg_idx], ios::binary);
            if(!instruction_file.is_open()) {
                throw runtime_error(string("Invalid parameter: \"") + argv[arg_idx] + "\"\n");
            }
        }
    }
    if (!instruction_file.is_open() || instruction_file.fail()) {
        throw runtime_error("Multiple or no trace files provided\n");
    }
}

void Pipeline::run() {
    while (1) {
        instruction_array[4] = instruction_array[3];
        instruction_array[3] = instruction_array[2];
        if (!stall) {
            instruction_array[2] = instruction_array[1];
            instruction_array[1] = instruction_array[0];
            if (end) {
                instruction_array[0] = Instruction{};
                if (ranges::all_of(instruction_array,[](Instruction instruction){
                    return instruction.opcode == NOP;
                }))
                    return;
            } else if (!instruction_file.read(reinterpret_cast<char*>(&instruction_array[0]),
            sizeof(Instruction))) {
                instruction_file.close();
                instruction_array[0] = Instruction{};
                end = true;
            }
        } else
            instruction_array[2] = Instruction{};

        ID();
        EX();
        /*MEM();
        WB();*/
        cycle_count++;
    }
}

int Pipeline::get_cycles() {
    return cycle_count;
}

// TD: Add other instructions as well, currently considering only CBR, ALU and LD
void Pipeline::ID() {
    if (check_depend(instruction_array[1], instruction_array[2])
        && (!FWD_EXE || instruction_array[2].opcode == OP_LD))
        stall = true;
    else if (check_depend(instruction_array[1], instruction_array[3]) && !FWD_MEM)
        stall = true;
    else
        stall = false;
}

void Pipeline::EX() {
    // TD: Implement
}