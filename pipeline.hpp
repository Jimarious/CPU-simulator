#pragma once

#include "instruction.hpp"
#include <optional>
#include <array>
#include <fstream>

class Pipeline {
public:
    Pipeline (int argc, char *argv[]);
    void run();
    int get_cycles();

private:
    int PIPE_WIDTH = 1;
    int BR_PRED = 0;
    bool FWD_EXE = false, FWD_MEM = false;
    bool stall = false;
    bool end = false;
    std::ifstream instruction_file;
    std::array<Instruction, 5> instruction_array;

    int cycle_count = 0;


    void ID();
    void EX();
    void MEM();
    void WB();
};
