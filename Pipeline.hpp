#pragma once

#include "Instruction.hpp"
#include <optional>
#include <array>

std::array<Instruction, 5> instruction_array;

class Pipeline {
private:
    int PIPE_WIDTH = 1;
    int BR_PRED = 0;
    bool FWD_EXE = false, FWD_MEM = false;
    bool stall = false;
    bool end = false;
    
public: 
    cycle(ifstream instruction_file) {
        instruction_array[4] = instruction_array[3];
        instruction_array[3] = instruction_array[2];
        if (!stall) {
            instruction_array[2] = instruction_array[1];
            instruction_array[1] = instruction_array[0];
            if (end) {
                instruction_array[0] = Instruction{};
                if (std::all_of(instruction_array,[](Instruction instruction){
                    return instruction.opcode == NOP;
                }))
                    return 0;
            } else if (!instruction_file.read(reinterpret_cast<char*>(&instruction_array[0]),
            sizeof(Instruction))) {
                instruction_file.close();
                instruction_array[0] = Instruction{};
                end = true;
            }
        } else
            instruction_array[2] = Instruction{};

        ID(stall, FWD_EXE, FWD_MEM);
        EX();
        MEM();
        WB();
    }
};
