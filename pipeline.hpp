#include "instruction.hpp"
#include <optional>
#include <array>
#include <fstream>
#include <boost/iostreams/filtering_stream.hpp>

class Pipeline {
public:
    Pipeline (int argc, char *argv[]);
    void run();
    int get_cycle_count();
    int get_instruction_count();
    int get_total_br_count();
    int get_mispred_br_count();

private:
    int PIPE_WIDTH = 1;
    int BR_PRED = 0;
    bool FWD_EXE = false, FWD_MEM = false;
    int min_stalled_lane;
    bool flush = false;
    bool end = false;
    boost::iostreams::filtering_istream instruction_file;
    std::vector<Instruction> instruction_array[5];

    int instruction_count = 0;
    int cycle_count = 0;
    int total_br_count = 0;
    int mispred_br_count = 0;

    void IF();
    void ID();
    void EX();
    void MEM();
    void WB();
};
