#include <array>
#include <cstdint>

class Gshare {
public:
    bool predict(uint16_t instruction_address_r12, uint8_t br_dir);
    Gshare();

private:
    uint16_t GHR = 0;
    std::array<uint8_t, 1028> PHT;
};