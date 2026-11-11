#include "pipeline.hpp"
#include <iostream>
#include <iomanip>

using namespace std;

int main(int argc, char *argv[]) {
    Pipeline pipeline(argc, argv);
    pipeline.run();

    cout << left << setw(22) << "LAB2_NUM_INST" << ":"
    << right << setw(22) << pipeline.get_instruction_count() << "\n";
    cout << left << setw(22) << "LAB2_NUM_CYCLES" << ":"
    << right << setw(22) << pipeline.get_cycle_count() << "\n";
    cout << left << setw(22) << "LAB2_CPI" << ":"
    << right << setw(22)
    << static_cast<float>(pipeline.get_cycle_count())/pipeline.get_instruction_count() << "\n";
    cout << left << setw(22) << "LAB2_BPRED_BRANCHES" << ":"
    << right << setw(22) << pipeline.get_total_br_count() << "\n";
    cout << left << setw(22) << "LAB2_BPRED_MISPRED" << ":"
    << right << setw(22) << pipeline.get_mispred_br_count() << "\n";
}