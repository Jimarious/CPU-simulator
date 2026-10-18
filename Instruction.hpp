#pragma once

enum class Opcode {
    ADD,
    LW,
    SW,
    BRANCH
};

class Instruction {
    Opcode opcode;
    int rs = -1;
    int rt = -1;
    int rd = -1;
};