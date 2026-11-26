#include "pipeline.hpp"
#include "cache.hpp"

class Simulator {
public:
    Simulator(int argc, char *argv[]);

private:
    std::array<std::optional<Pipeline>, 2> cores;
    int num_traces = 0;
    std::optional<Cache> L2_cache;
};