#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>

enum Opcode {
    MOV, ADD, J //, nullop
};

class Instruction {
    public:
        Opcode opcode;
        int reg[3];
};

// CPU class: Instruction stage[5], IF(), ID(), ...

int main () {
    std::ifstream instructions_file("instructions");
    std::string opcode_str, reg_str;
    while(instructions_file >> opcode_str) {
        while 
    }
    
    
    
    // CPU.IF()
    // 

}