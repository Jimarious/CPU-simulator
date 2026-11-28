#include "pipeline.hpp"
#include <iostream>
#include <fstream>
#include <charconv>
#include <cstring>
#include <algorithm>
#include <iomanip>
#include <filesystem>

using namespace std;

Pipeline::Pipeline(int argc, char *argv[]) {
    for (int arg_idx = 1; arg_idx < argc; arg_idx++) {
        if (!strcmp(argv[arg_idx], "-pipewidth")) {
            PIPE_WIDTH = parse_number_arg(argv, arg_idx, argc, 1, 8);
        } else if (!strcmp(argv[arg_idx], "-bpredpolicy")) {
            PIPE_WIDTH = parse_number_arg(argv, arg_idx, argc, 0, 2);
        } else if (!strcmp(argv[arg_idx], "-enableexefwd")) {
            FWD_EXE = true;
        } else if (!strcmp(argv[arg_idx], "-enablememfwd")) {
            FWD_MEM = true;
        } else if (!strcmp(argv[arg_idx], "-simcache")) {
            enable_L1_cache = true;
        } else if (!strcmp(argv[arg_idx], "-totalblocks")) {
            L1_cache->totalblocks = parse_number_arg(argv, arg_idx, argc, 1, UINT32_MAX);
            enable_L1_cache = true;
        } else if (!strcmp(argv[arg_idx], "-associativity")) {
            L1_cache->associativity = parse_number_arg(argv, arg_idx, argc, 1, 16);
            enable_L1_cache = true;
        } else if (!strcmp(argv[arg_idx], "-blocksize")) {
            L1_cache->blocksize = parse_number_arg(argv, arg_idx, argc, 1, 1024);
            enable_L1_cache = true;
        } else if (!strcmp(argv[arg_idx], "-evictionpolicy")) {
            L1_cache->totalblocks = parse_number_arg(argv, arg_idx, argc, 0, 2);
            enable_L1_cache = true;
        } else if (!std::filesystem::is_regular_file(argv[arg_idx])) {
            throw runtime_error(string("Invalid argument or non existing file: \"")
            + argv[arg_idx] + "\"\n");
        }
    }
    if (enable_L1_cache) L1_cache.emplace();
    for (int i=0; i<5; i++)  instruction_array[i].resize(PIPE_WIDTH);
    min_stalled_lane = PIPE_WIDTH;
}

void Pipeline::update_stall_for_rs1(int lane_young) {
    for (int lane_old=lane_young-1; lane_old>=0; lane_old--) {
        if (instruction_array[0][lane_old].rd_used
        && instruction_array[0][lane_young].rs1 == instruction_array[0][lane_old].rd) {
            min_stalled_lane = min(min_stalled_lane, lane_young);
            return;
        }
    }
    for (int lane_old=PIPE_WIDTH-1; lane_old>=0; lane_old--) {     
        if (instruction_array[1][lane_old].rd_used
        && instruction_array[0][lane_young].rs1 == instruction_array[1][lane_old].rd) {
            if (!FWD_EXE || instruction_array[1][lane_old].opcode == OP_LD)
                min_stalled_lane = min(min_stalled_lane, lane_young);
            return;
        }
    }
    for (int lane_old=PIPE_WIDTH-1; lane_old>=0; lane_old--) { 
        if (instruction_array[2][lane_old].rd_used
        && instruction_array[0][lane_young].rs1 == instruction_array[2][lane_old].rd) {
            if (!FWD_MEM)
                min_stalled_lane = min(min_stalled_lane, lane_young);
            return;
        }
    }
}

void Pipeline::update_stall_for_rs2(int lane_young) {
    for (int lane_old=lane_young-1; lane_old>=0; lane_old--) {
        if (instruction_array[0][lane_old].rd_used
        && instruction_array[0][lane_young].rs2 == instruction_array[0][lane_old].rd) {
            min_stalled_lane = min(min_stalled_lane, lane_young);
            return;
        }
    }
    for (int lane_old=PIPE_WIDTH-1; lane_old>=0; lane_old--) {     
        if (instruction_array[1][lane_old].rd_used
        && instruction_array[0][lane_young].rs2 == instruction_array[1][lane_old].rd) {
            if (!FWD_EXE || instruction_array[1][lane_old].opcode == OP_LD)
                min_stalled_lane = min(min_stalled_lane, lane_young);
            return;
        }
    }
    for (int lane_old=PIPE_WIDTH-1; lane_old>=0; lane_old--) { 
        if (instruction_array[2][lane_old].rd_used
        && instruction_array[0][lane_young].rs2 == instruction_array[2][lane_old].rd) {
            if (!FWD_MEM)
                min_stalled_lane = min(min_stalled_lane, lane_young);
            return;
        }
    }
}

void Pipeline::update_stall_for_cc(int lane_young) {
    for (int lane_old=lane_young-1; lane_old>=0; lane_old--) {
        if (instruction_array[0][lane_old].cc_w) {
            min_stalled_lane = min(min_stalled_lane, lane_young);
            return;
        }
    }
    for (int lane_old=PIPE_WIDTH-1; lane_old>=0; lane_old--) {
        if (instruction_array[1][lane_old].cc_w) {
            if (!FWD_EXE || instruction_array[1][lane_old].opcode == OP_LD)
                min_stalled_lane = min(min_stalled_lane, lane_young);
            return;
        }
    }
    for (int lane_old=PIPE_WIDTH-1; lane_old>=0; lane_old--) {
        if (instruction_array[2][lane_old].cc_w) {
            if (!FWD_MEM)
                min_stalled_lane = min(min_stalled_lane, lane_young);
            break;
        }
    }
}

void Pipeline::run() { // stalls are one stage earlier so that only IF needs sort
    while (1) {
        instruction_array[4] = instruction_array[3];
        instruction_array[3] = instruction_array[2];
        instruction_array[2] = instruction_array[1];
        for (int lane=0; lane<min_stalled_lane; lane++) {
            instruction_array[1][lane] = instruction_array[0][lane];
        }
        for (int lane=min_stalled_lane; lane<PIPE_WIDTH; lane++) {
            instruction_array[1][lane] = Instruction{};
        }
        IF();
        if (end && ranges::all_of(instruction_array, [](vector<Instruction> stage){
            return ranges::all_of(stage, [](Instruction instruction){
                return instruction.opcode == NOP;
            });
        })) return;
        ID();
        MEM();

        cycle_count++;
    }
}

void Pipeline::IF() {
    for (int lane=0; lane<PIPE_WIDTH-min_stalled_lane; lane++) {
        instruction_array[0][lane] = instruction_array[0][min_stalled_lane+lane];
    }
    for (int lane=PIPE_WIDTH-min_stalled_lane; lane<PIPE_WIDTH; lane++) {
        if (end || flush) {
            instruction_array[0][lane] = Instruction{};
        } else if (!trace_file.read(reinterpret_cast<char*>(&instruction_array[0][lane]),
        sizeof(Instruction))) {
            instruction_array[0][lane] = Instruction{};
            end = true;
        } else {
            instruction_count++;
            if (instruction_array[0][lane].opcode == OP_CBR) {
                total_br_count++;
                if ((BR_PRED == 1 && !instruction_array[0][lane].br_dir) || (BR_PRED == 2
                && gshare->predict(instruction_array[0][lane].instruction_address & 0xFFF,
                instruction_array[0][lane].br_dir) != instruction_array[0][lane].br_dir)) {
                    mispred_br_count++;
                    instruction_array[0][lane].br_dir |= 2; // 10 indicates misprediction for MEM
                    flush = true;
                }
            }
        }
    }
}

// TD: OP_OTHER outputs at EXE, after this make a trace file generator that encodes actual consume-produce stages.
void Pipeline::ID() {
    min_stalled_lane = PIPE_WIDTH;
    for (int lane_young=0; lane_young<PIPE_WIDTH; lane_young++) {
        if (instruction_array[0][lane_young].rs1_used)
            update_stall_for_rs1(lane_young);
        if (instruction_array[0][lane_young].rs2_used) 
            update_stall_for_rs2(lane_young);
        if (instruction_array[0][lane_young].cc_r)
            update_stall_for_cc(lane_young);
    }
}

void Pipeline:: MEM() {
    for (int lane=0; lane<PIPE_WIDTH; lane++) {
        if (instruction_array[3][lane].opcode == OP_CBR && instruction_array[3][lane].br_dir & 0b10) {
            flush = false;
            break;
        }
    }
}

void Pipeline::print_stats() {
    if (L1_cache.has_value()) {
        for (int core_idx=0; core_idx<2; core_idx++) {
            // TD: print stuff
        }
    } else {
        cout << left << setw(22) << "LAB2_NUM_INST" << ":"
        << right << setw(22) << instruction_count << "\n";
        cout << left << setw(22) << "LAB2_NUM_CYCLES" << ":"
        << right << setw(22) << cycle_count << "\n";
        cout << left << setw(22) << "LAB2_CPI" << ":"
        << right << setw(22)
        << static_cast<float>(cycle_count)/instruction_count << "\n";
        cout << left << setw(22) << "LAB2_BPRED_BRANCHES" << ":"
        << right << setw(22) << total_br_count << "\n";
        cout << left << setw(22) << "LAB2_BPRED_MISPRED" << ":"
        << right << setw(22) << mispred_br_count << "\n";

        cout << left << setw(26) << "CYCLES" << ":"
        << right << setw(22) << cycle_count << "\n";
    }

    return;
}

int parse_number_arg(char *argv[], int &arg_idx, int argc, int left_bound, int right_bound) {
    int num;
    if (++arg_idx == argc) {
        throw runtime_error(std::string("Expected value for ") + argv[arg_idx-1] + "\n");
    }
    auto [ptr, err] = from_chars(argv[arg_idx], argv[arg_idx]+strlen(argv[arg_idx]), num);
    if (err != errc{} || ptr != argv[arg_idx]+strlen(argv[arg_idx])
        || num < left_bound || num > right_bound)  {
        throw runtime_error(std::string("Invalid") + argv[arg_idx-1] + "value\n");
    }
    return num;
}