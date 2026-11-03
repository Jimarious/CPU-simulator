#include "instruction.hpp"
#include <optional>
#include <array>
#include <fstream>
#include <boost/iostreams/filtering_stream.hpp>

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
    boost::iostreams::filtering_istream instruction_file;
    std::array<Instruction, 5> instruction_array;

    int cycle_count = 0;


    void ID();
    void EX();
    void MEM();
    void WB();
};
