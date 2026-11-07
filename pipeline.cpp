#include "pipeline.hpp"
#include <iostream>
#include <fstream>
#include <charconv>
#include <cstring>
#include <algorithm>
#include <filesystem>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/device/file.hpp>

using namespace std;

int check_depend(Instruction young, Instruction old) {
    return old.rd_used && ((young.rs1_used && young.rs1 == old.rd)
    || (young.rs2_used && young.rs2 == old.rd))
    || (young.cc_r && old.cc_w);
}


Pipeline::Pipeline(int argc, char *argv[]) {
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
        } else if (std::filesystem::is_regular_file(argv[arg_idx])) {
            if (instruction_file.is_complete()) {
                throw runtime_error("Multiple files provided\n");
            }
            boost::iostreams::file_source instruction_file_gz(argv[arg_idx], ios::binary);
            if(!instruction_file_gz.is_open()) {
                throw runtime_error(string("Problem opening: \"") + argv[arg_idx] + "\"\n");
            }
            instruction_file.push(boost::iostreams::gzip_decompressor());
            instruction_file.push(instruction_file_gz);
        } else {
            throw runtime_error(string("Invalid argument or non existing file: \"")
            + argv[arg_idx] + "\"\n");
        }
    }
    if (!instruction_file.is_complete()) {
        throw runtime_error("No trace file provided\n");
    }
    for (int i=0; i<5; i++)  instruction_array[i].resize(PIPE_WIDTH);
    min_stalled_lane = PIPE_WIDTH;
}

void Pipeline::run() {
    while (1) {
        instruction_array[4] = instruction_array[3];
        instruction_array[3] = instruction_array[2];
        for (int lane=0; lane<min_stalled_lane; lane++) {
            instruction_array[2][lane] = instruction_array[1][lane];
            instruction_array[1][lane] = instruction_array[0][lane];
            if (end) {
                instruction_array[0][lane] = Instruction{};
                if (!lane && ranges::all_of(instruction_array, [](vector<Instruction> stage){
                    return ranges::all_of(stage, [](Instruction instruction){
                        return instruction.opcode == NOP;
                    });
                }))
                    return;
            } else if (!instruction_file.read(reinterpret_cast<char*>(&instruction_array[0][lane]),
            sizeof(Instruction))) {
                instruction_array[0][lane] = Instruction{};
                end = true;
            }
        }
        for (int lane=min_stalled_lane; lane<PIPE_WIDTH; lane++) {
            instruction_array[2][lane] = Instruction{};
        }
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

// TD: OP_OTHER outputs at EXE, after this make a trace file generator that encodes actual consume-produce stages.
void Pipeline::ID() {
    // TD: Now there are are data hazards in the same stage as well, also implement loop over lanes
    for (int lane_young=0; lane_young<PIPE_WIDTH; lane_young++) {
        for (int lane_old=0; lane_old<PIPE_WIDTH; lane_old++) {
            if (lane_young > lane_old
            && check_depend(instruction_array[1][lane_young], instruction_array[1][lane_old]))
                min_stalled_lane = min(min_stalled_lane, lane_young);
            if (check_depend(instruction_array[1][lane_young], instruction_array[2][lane_old])
            && (!FWD_EXE || instruction_array[2][lane_old].opcode == OP_LD))
                min_stalled_lane = min(min_stalled_lane, lane_young);
            else if (check_depend(instruction_array[1][lane_young], instruction_array[3][lane_old]) && !FWD_MEM)
                min_stalled_lane = min(min_stalled_lane, lane_young);
            else
                min_stalled_lane = PIPE_WIDTH;
        }
    }
}

void Pipeline::EX() {
    // TD: Implement
}