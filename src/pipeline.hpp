#include "instruction.hpp"
#include "gshare.hpp"
#include "cache.hpp"
#include <optional>
#include <boost/iostreams/filtering_stream.hpp>

class Pipeline {
public:
    Pipeline (int argc, char *argv[]);
    boost::iostreams::filtering_istream trace_file;
    void run();
    void print_stats();

private:
    int PIPE_WIDTH = 1;
    int BR_PRED = 0;
    bool FWD_EXE = false, FWD_MEM = false;
    int min_stalled_lane;
    bool flush = false;
    bool end = false;
    std::vector<Instruction> instruction_array[5];
    std::optional<Gshare> gshare;
    std::optional<bool> enable_L1_cache;
    std::optional<Cache> L1_cache;

    int instruction_count = 0;
    int cycle_count = 0;
    int total_br_count = 0;
    int mispred_br_count = 0;

    void IF();
    void ID();
    void MEM();

    void update_stall_for_rs1(int lane_young);
    void update_stall_for_rs2(int lane_young);
    void update_stall_for_cc(int lane_young);
};
