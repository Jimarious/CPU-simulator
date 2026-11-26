#include "pipeline.hpp"
#include "simulator.hpp"
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/device/file.hpp>
#include <filesystem>

using namespace std;

int main(int argc, char *argv[]) {
    Pipeline pipeline(argc, argv);
    pipeline.run();
    pipeline.print_stats();
}

Simulator::Simulator(int argc, char *argv[]) {
    for (int arg_idx = 1; arg_idx < argc; arg_idx++) {
        if (std::filesystem::is_regular_file(argv[arg_idx])) {
            if (num_traces >= 2) 
                throw runtime_error("Too many trace files, max number of cores is 2\n");
            boost::iostreams::file_source trace_file_gz(argv[arg_idx], ios::binary);
            if(!trace_file_gz.is_open()) {
                throw runtime_error(string("Problem opening: \"") + argv[arg_idx] + "\"\n");
            }
            cores[num_traces]->trace_file.push(boost::iostreams::gzip_decompressor());
            cores[num_traces]->trace_file.push(trace_file_gz);
        }
    }
    if (!num_traces) {
        throw runtime_error("No trace file provided\n");
    }
}