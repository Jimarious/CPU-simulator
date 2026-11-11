#include "gshare.hpp"

Gshare::Gshare() {
    std::fill(PHT.begin(), PHT.end(), 0b10101010);
}

bool Gshare::predict(uint16_t instruction_address_r12, uint8_t br_dir) {
    uint16_t PHT_index = instruction_address_r12 ^ GHR;
    uint8_t PHT_2bit_entry_offset = (PHT_index & 0b11) * 2;
    uint8_t status = PHT[PHT_index >> 2] >> PHT_2bit_entry_offset & 0b11;
    bool prediction = status > 1;

    GHR = GHR << 1 & 0xFFF | br_dir;
    if (br_dir) {
        if (status != 3)
            PHT[PHT_index >> 2] += 1 << PHT_2bit_entry_offset;
    }
    else if (status)
        PHT[PHT_index >> 2] -= 1 << PHT_2bit_entry_offset;

    return prediction;
}