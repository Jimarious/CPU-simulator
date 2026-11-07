#include "pipeline.hpp"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
    Pipeline pipeline(argc, argv);
    pipeline.run();

    cout << pipeline.get_cycles() << "\n";
}